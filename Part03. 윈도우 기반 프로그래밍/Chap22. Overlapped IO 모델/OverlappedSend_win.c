/*
SOCKET WSASocket(int af, int type, int protocol, LPWSAPROTOCOL_INFO lpProtocolInfo, GROUP g, DWORD dwFlags);
params는 프로토콜 체계 정보, 소켓 데이터 전송방식, 두 소켓사이 프로토콜, 소켓의 특성정보(null), 예약됨(0), 소켓의 속성정보(WSA_FLAG_OVERLAPPED)를 전달 

int WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTUNE lpCompletionRoutine);
소켓핸들, WSABUF배열, 길이, 전송바이트수저장변수주소, 데이터전송특성, WSAOVERLAPPED구조체 주소(전송확인), 함수주소값(전송확인) 

typedef struct _WSAOVERLAPPED{
	DWORD Internal;//OS내부적사용 
	DWORD InternalHigh;//OS내부적사용 
	DWORD Offset;//예약됨 
	DWORD OffsetHigh;//예약된 
	WSAEVENT hEvent;//얘만 조작함. 
} WSAOVERLAPPED, *LPWSAOVERLAPPED;

BOOL WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOl fWait, LPDWORD lpdwFlags);
Overlapped IO가 진행된 소켓의 핸들, WSAOVERLAPPED구조체 주소, 바이트크기 저장할 곳, wait여부, 부수적인 정보 얻기용

int WSARecv(~) 

int WSAGetLastError(void);//오류상황에 대한 상태 갑(오류의 원인을 알리는 값) 반환 WSA_IO_PENDING(보류중)은 데이터가 전송되고 있음을 뜻함. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char *msg);

int main(int argc, char *argv[]){//클라이언트(전송) 
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;
	
	WSABUF dataBuf;
	char msg[]="Network is Computer!";//전송할 메시지. 곧 WSABUF로 변환 예정. 
	int sendBytes=0;//전송한 바이트의 크기가 기록될 변수. 
	
	WSAEVENT evObj;//이벤트 for WSAOVERLAPPED.hEvent 
	WSAOVERLAPPED overlapped;//OVERLAPPED_IO를 위한 구조체. 만약 WSASend()로 둘 이상의 곳에 데이터를 전송하는 경우에 각각 준비해야한다. 
	
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hSocket=WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);//일반적인 소켓 생성과 비슷하지만, 마지막에 overlap플래그를 전달한다. 그렇지않으면 블로킹모드로 작동하는 읿반적인 소켓이 생성된다. 
	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family=AF_INET;
	sendAdr.sin_addr.s_addr=inet_addr(argv[1]);
	sendAdr.sin_port=htons(atoi(argv[2]));
	
	if(connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
		
	evObj=WSACreateEvent();//이벤트 오브젝트 생성(이전챕터에서 생성한 것과 마찬가지로 manual-mode, un-signaled 이벤트 생성) 
	memset(&overlapped, 0, sizeof(overlapped));//WSAOVERLAPPED구조체 0으로 초기화 
	overlapped.hEvent=evObj;//이벤트 할당 
	dataBuf.len=strlen(msg)+1;//WSABUF세팅(WSASend는 WSABUF를 사용한다_Gatter/Scatter IO) 
	dataBuf.buf=msg;
	
	if(WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL)==SOCK_ERROR){//소켓, 전송할 WSABUF배열, 크기, 전송바이트담을곳, 예약, WSAOVERLAPPED구조체, 옵션) 
		//SOCK_ERROR은 즉각적인 반환이 아닌 데이터 전송중일 경우. 
		if(WSAGetLastError()==WSA_TO_PENDING){//데이터의 전송이 아직 진행중인 경우 
			puts("Background data send");//백그라운드에서 데이터가 전송되고있음을 알리고 
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);//이벤트가 활성화 될 때까지 기다린다. 
			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);//결과를 얻는다.? 
		} else{
			ErrorHandling("WSASend() error");
		}
	}//else를 따로 빼지 않은 이유는 SOCK_ERROR을 반환하지 않았다면 정상적인 0으로서 데이터의 크기가 작아 호출과 동시에 반환되어 즉각적으로 sendBytes변수에 전송한 바이트수가 저장되어있다. 
	
	printf("Send data size: %d\n", sendBytes);
	WSACloseEvent(evObj);
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
