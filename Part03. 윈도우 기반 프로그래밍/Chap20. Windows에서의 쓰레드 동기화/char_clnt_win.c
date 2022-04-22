#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <process.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

unsigned WINAPI SendMsg(void * arg);
unsigned WINAPI RecvMsg(void * arg);
void ErrorHandling(char * msg);

char name[NAME_SIZE]="[DEFAULT]";//이름이 정해지지 않았을 경우를 대비. in client 
char msg[BUF_SIZE];

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSock;
	SOCKADDR_IN servAdr;
	HANDLE hSndThread, hRcvThread;
	if(argc!=4){
		printf("Usage: %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	sprintf(name, "[%s]", argv[3]);//main 인자로 전달된 client 이름 저장 
	hSock=socket(PF_INET, SOCK_STREAM, 0);//TCP/IP
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr(argv[1]);
	servAdr.sin_port=htons(atoi(argv[2]));
	
	if(connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("connect() error");
		
	hSndThread=(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);//SendMsg과정과  
	hRcvThread=(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);//RecvMsg과정을 별개의 쓰레드로 병행실행 
	
	WaitForSingleObject(hSndThread, INFINITE);//수신할 메시지가 없을 경우 종료하는 것을 기달. 
	WaitForSingleObject(hRcvThread, INFINITE);//마찬가지 
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI SendMsg(void * arg){
	SOCKET hSock=*((SOCKET*)arg);//인자로 받은 소켓을 저장 
	char nameMsg[NAME_SIZE+BUF_SIZE];
	while(1){
		fgets(msg, BUF_SIZE, stdin);//stdin에서 msg를 입력받고 
		if(!strcmp(msg, "q\n")||!strcmp(msg, "Q\n")){
			closesocket(hSock);
			exit(0);
		}
		sprintf(nameMsg, "%s %s", name ,msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);//서버에 메시지를 전달한다. 
	}
	return 0;
}

unsigned WINAPI RecvMsg(void * arg){
	int hSock=*((SOCKET*)arg);//인자로 받은 소켓을 저장 
	char nameMsg[NAME_SIZE+BUF_SIZE];
	int strLen;
	while(1){
		strLen=recv(hSock, nameMsg, NAME_SIZE+BUF_SIZE-1, 0);//서버로부터 수신받은 메시지를 저장한다. 
		if(strLen==-1)
			return -1;
		nameMsg[strLen]=0;//null처리후 
		fputs(nameMsg, stdout);//stdout에 출력. 
	}
	return 0;
}

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
