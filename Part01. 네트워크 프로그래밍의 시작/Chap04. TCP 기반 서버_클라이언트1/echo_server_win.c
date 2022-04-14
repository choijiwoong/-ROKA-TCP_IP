//윈도우 기반으로 변경 전 확인해야할 것: WSAStartup & WSACleanup, 자료형과 변수의 이름을 윈도우 식으로, read & write->recv & send, close->closesocket
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	char message[BUF_SIZE];
	int strLen, i;
	
	SOCKADDR_IN servAdr, clntAdr;
	int clntAdrSize;
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)//Window소켓 세팅 
		ErrorHandling("WSAStartup() error!");
	
	hServSock=socket(PF_INET, SOCK_STREAM, 0);//handle(descriptor in linux) 
	if(hServSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
		
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)//SOCKADDR사용 
		ErrorHandling("bind() error");
		
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
	
	clntAdrSize=sizeof(clntAdr);
	
	for(i=0; i<5; i++){
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);//서버에 clntadr로 접근하여 clnt socket생성 및 연결 
		if(hClntSock==-1)
			ErrorHandling("accept() error");
		else
			printf("Connected client %d\n", i+1);
		
		while((strLen=recv(hClntSock, message ,BUF_SIZE, 0))!=0)//client로부터 메시지를 받으면 (receive) 
			send(hClntSock, message, strLen, 0);//client에 수신받은 메시지를 전달. 
			
		closesocket(hClntSock);
	}
	closesocket(hServSock);
	WSACleanup();
	return 0;
} 

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
