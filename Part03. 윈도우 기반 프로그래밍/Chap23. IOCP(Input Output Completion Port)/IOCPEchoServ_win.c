#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <winsock2.h>
#include <windows.h>

#define BUF_SIZE 100
#define READ 3
#define WRITE 5

typedef struct{//클라이언트 정보를 묶어서 저장 
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct{//OVERLAPPED, WSABUF, buffer, rwMode를 묶어 저장(nasm에서 배운 당연한 직관. struct주소는 첫맴버 주소)
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	int rwMode;
} PER_IO_DATA, *LPPER_IO_DATA;

DWORD WINAPI EchoThreadMain(LPVOID CompletionPortID);
void ErrorHandling(char *message);

int main(int argc, char * argv[]){
	WSADATA wsaData;
	HANDLE hComPort;
	SYSTEM_INFO sysInfo;//현재 실행중인 시스템 정보를 담기 위한 구조체 
	LPPER_IO_DATA ioInfo;
	LPPER_HANDLE_DATA handleInfo;
	
	SOCKET hServSock;
	SOCKADDR_IN servAdr;
	int recvBytes, i, flags=0;
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hComPort=CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);//CP오브젝트의 생성 
	GetSyetemInfo(&sysInfo);//실행중인 시스템의 정보를 얻기 위함 
	for(i=0; i<sysInfo.dwNumberOfProcessers; i++)//CPU의 수가 저장된다. 
		_beginthreadex(NULL, 0, EchoThreadMain, (LPVOID)hComPort, 0, NULL);//쓰레드생성시 CP오브젝트의 핸들을 전달. 
		
	hServSock=WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAdr.sin_port=htons(atoi(argv[1]));
	
	bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr));
	listen(hServSock, 5);
	
	while(1){
		SOCKET hClntSock;
		SOCKADDR_IN clntAdr;
		int addrLen=sizeof(clntAdr);
		
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &addrLen);//클라이언트 요청 수락 
		handleInfo=(LPPER_HANDLE_DATA)malloc(sizeof(PER_HANDLE_DATA));//사용자 정의 구조체 동적할당 
		handleInfo->hClntSock=hClntSock;//클라이언트 소켓 핸들 저장 
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);//사용자 정의 구조체의 client address를 초기화? 
		
		CreateIoCompletionPort((HANDLE)hClntSock, hComPort, (DWORD)handleInfo, 0);//CP오브젝트와 소켓의 연결. Overlapped IO가 완료될 때, CP오브젝트의 정보가 삽입되고, GetQueued...함수가 반환된다. 3번째 인자는 바로위에서 동적할당하고 초기화한 사용자 정의 함수이다. 
		ioInfo=(LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));//입출력에 필요한 정보를 가진 사용자 정의 구조체 동적할당 
		memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));//OVERLAPPED초기화 
		ioInfo->wsaBuf.len=BUF_SIZE;
		ioInfo->wsaBuf.buf=ioInfo->buffer;//버퍼설정 
		ioInfo->rwMode=READ;//읽기모드(서버) 
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, &recvBytes, &flags, &(ioInfo->overlapped), NULL);
	}
	return 0;
}

DWORD WINAPI EchoThreadMain(LPVOID pComPort){
	HANDLE hComPort=(HANDLE)pComPort;
	SOCKET sock;
	DWORD bytesTrans;
	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	DWORD flags=0;
	
	while(1){
		GetQueuedCompletionStatus(hComPort, &bytesTrans, (LPDWORD)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);//가 호출되는 쓰레드를 CP오브젝트에 할당된 스레드라고 한다. 
		//IO가 완료되고, 이에대한 정보가 등록되었을 경우 반환하는데, 세번째, 네번쨰 인자를 통해 두가지 정보를 얻는다. 
		sock=handleInfo->hClntSock;
		
		if(ioInfo->rwMode==READ){//ioInfo값은 OVERLAPPED구조체 주소이지만, PER_IO_DATA구조체변수의 주소값이기도 하다. 
			puts("message received!");
			if(bytesTrans==0){
				closesocket(sock);
				free(handleInfo);
				free(ioInfo);
				continue;
			}
			
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len=bytesTrans;
			ioInfo->rwMode=WRITE;//클라이언트용 IO정보 세팅 
			WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, (&ioInfo->overlapped), NULL);//echo예정 
			
			ioInfo=(LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));//매번 따로 IO_info를 전달해준다. 
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len=BUF_SIZE;
			ioInfo->wsaBuf.buf=ioInfo->buffer;//PER_IO_DATA의 buffer을 wsaBuf의 buf로 할당 
			ioInfo->rwMode=READ;
			WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);//수신. 
		} else{
			puts("message sent!");
			free(ioInfo);
		}
	}
	return 0;
} 

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
