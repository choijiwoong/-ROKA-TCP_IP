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

char name[NAME_SIZE]="[DEFAULT]";//�̸��� �������� �ʾ��� ��츦 ���. in client 
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
		
	sprintf(name, "[%s]", argv[3]);//main ���ڷ� ���޵� client �̸� ���� 
	hSock=socket(PF_INET, SOCK_STREAM, 0);//TCP/IP
	
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr(argv[1]);
	servAdr.sin_port=htons(atoi(argv[2]));
	
	if(connect(hSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("connect() error");
		
	hSndThread=(HANDLE)_beginthreadex(NULL, 0, SendMsg, (void*)&hSock, 0, NULL);//SendMsg������  
	hRcvThread=(HANDLE)_beginthreadex(NULL, 0, RecvMsg, (void*)&hSock, 0, NULL);//RecvMsg������ ������ ������� ������� 
	
	WaitForSingleObject(hSndThread, INFINITE);//������ �޽����� ���� ��� �����ϴ� ���� ���. 
	WaitForSingleObject(hRcvThread, INFINITE);//�������� 
	closesocket(hSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI SendMsg(void * arg){
	SOCKET hSock=*((SOCKET*)arg);//���ڷ� ���� ������ ���� 
	char nameMsg[NAME_SIZE+BUF_SIZE];
	while(1){
		fgets(msg, BUF_SIZE, stdin);//stdin���� msg�� �Է¹ް� 
		if(!strcmp(msg, "q\n")||!strcmp(msg, "Q\n")){
			closesocket(hSock);
			exit(0);
		}
		sprintf(nameMsg, "%s %s", name ,msg);
		send(hSock, nameMsg, strlen(nameMsg), 0);//������ �޽����� �����Ѵ�. 
	}
	return 0;
}

unsigned WINAPI RecvMsg(void * arg){
	int hSock=*((SOCKET*)arg);//���ڷ� ���� ������ ���� 
	char nameMsg[NAME_SIZE+BUF_SIZE];
	int strLen;
	while(1){
		strLen=recv(hSock, nameMsg, NAME_SIZE+BUF_SIZE-1, 0);//�����κ��� ���Ź��� �޽����� �����Ѵ�. 
		if(strLen==-1)
			return -1;
		nameMsg[strLen]=0;//nulló���� 
		fputs(nameMsg, stdout);//stdout�� ���. 
	}
	return 0;
}

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
