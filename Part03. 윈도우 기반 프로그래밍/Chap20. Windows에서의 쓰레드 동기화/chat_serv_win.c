#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

unsigned WINAPI HandleClnt(void * arg);
void SendMsg(char * msg, int len);
void ErrorHandling(char * msg);

int clntCnt=0;
SOCKET clntSocks[MAX_CLNT];
HANDLE hMutex;

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSz;
	HANDLE hThread;//쓰레드 핸들저장용 
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
	
	hMutex=CreateMutex(NULL, FALSE, NULL);//뮤텍스 생성(오브젝트 현재 Signal상태) 
	hServSock=socket(PF_INET, SOCK_STREAM, 0);//TCP/IP 
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
		
	while(1){
		clntAdrSz=sizeof(clntAdr);
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);//클라이언트 소켓 연결과 생성 
		
		WaitForSingleObject(hMutex, INFINITE);//Critical section SYN
		clntSocks[clntCnt++]=hClntSock;//클라이언트소켓(핸들)을 clntSocks변수에 저장 
		ReleaseMutex(hMutex);//Critical section FIN
		
		hThread=(HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);//쓰레드 생성 및 HandleClnt실행. 인자로 hClntSock전달 
		printf("Connected client IP: %s\n", inet_ntoa(clntAdr.sin_addr));//클라이언트 IP출력 
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI HandleClnt(void * arg){//클라이언트 소켓 핸들 
	SOCKET hClntSock=*((SOCKET*)arg);
	int strLen=0, i;
	char msg[BUF_SIZE];
	
	while((strLen=recv(hClntSock, msg, sizeof(msg), 0))!=0)//해당 클라이언트로부터 receive할 데이터가 있다면 msg에 버퍼로서 저장하며 
		SendMsg(msg, strLen);//Client모두에게 전송하는 SendMsg를 수행.  
		
	WaitForSingleObject(hMutex, INFINITE);//클라이언트 소멸을 위한 Critical section 
	for(i=0; i<clntCnt; i++){//만약 더이상 클라이언트 소켓으로부터 받을 게 없다면 클라이언트 목록 clntSocks에서 삭제. 
		if(hClntSock==clntSocks[i]){
			while(i++<clntCnt-1)
				clntSocks[i]=clntSocks[i+1];
			break;
		}
	}
	clntCnt--;
	ReleaseMutex(hMutex);//critical section FIN
	closesocket(hClntSock);
	return 0;
}

void SendMsg(char * msg, int len){
	int i;
	WaitForSingleObject(hMutex, INFINITE);//critical section SYN
	for(i=0; i<clntCnt; i++)//모든 클라이언트에게 
		send(clntSocks[i], msg, len, 0);//인자로 받은 msg 전송 
	ReleaseMutex(hMutex);//critical section FIN
}

void ErrorHandling(char * msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
