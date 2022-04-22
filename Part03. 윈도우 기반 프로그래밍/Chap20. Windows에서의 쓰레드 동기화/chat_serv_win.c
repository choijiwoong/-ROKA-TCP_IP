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
	HANDLE hThread;//������ �ڵ������ 
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
	
	hMutex=CreateMutex(NULL, FALSE, NULL);//���ؽ� ����(������Ʈ ���� Signal����) 
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
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSz);//Ŭ���̾�Ʈ ���� ����� ���� 
		
		WaitForSingleObject(hMutex, INFINITE);//Critical section SYN
		clntSocks[clntCnt++]=hClntSock;//Ŭ���̾�Ʈ����(�ڵ�)�� clntSocks������ ���� 
		ReleaseMutex(hMutex);//Critical section FIN
		
		hThread=(HANDLE)_beginthreadex(NULL, 0, HandleClnt, (void*)&hClntSock, 0, NULL);//������ ���� �� HandleClnt����. ���ڷ� hClntSock���� 
		printf("Connected client IP: %s\n", inet_ntoa(clntAdr.sin_addr));//Ŭ���̾�Ʈ IP��� 
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI HandleClnt(void * arg){//Ŭ���̾�Ʈ ���� �ڵ� 
	SOCKET hClntSock=*((SOCKET*)arg);
	int strLen=0, i;
	char msg[BUF_SIZE];
	
	while((strLen=recv(hClntSock, msg, sizeof(msg), 0))!=0)//�ش� Ŭ���̾�Ʈ�κ��� receive�� �����Ͱ� �ִٸ� msg�� ���۷μ� �����ϸ� 
		SendMsg(msg, strLen);//Client��ο��� �����ϴ� SendMsg�� ����.  
		
	WaitForSingleObject(hMutex, INFINITE);//Ŭ���̾�Ʈ �Ҹ��� ���� Critical section 
	for(i=0; i<clntCnt; i++){//���� ���̻� Ŭ���̾�Ʈ �������κ��� ���� �� ���ٸ� Ŭ���̾�Ʈ ��� clntSocks���� ����. 
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
	for(i=0; i<clntCnt; i++)//��� Ŭ���̾�Ʈ���� 
		send(clntSocks[i], msg, len, 0);//���ڷ� ���� msg ���� 
	ReleaseMutex(hMutex);//critical section FIN
}

void ErrorHandling(char * msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
