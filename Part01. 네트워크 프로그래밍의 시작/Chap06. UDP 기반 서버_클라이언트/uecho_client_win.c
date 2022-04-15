#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET sock;
	char message[BUF_SIZE];
	int strLen;
	
	SOCKADDR_IN servAdr;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	sock=socket(PF_INET, SOCK_DGRAM, 0);//UDP 소켓 
	if(sock==INVALID_SOCKET)
		ErrorHandling("socket() error");
		
	memset(&servAdr, 0, sizeof(servAdr));//인자로 얻은 서버 주소와 포트를 초기화 
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr(argv[1]);
	servAdr.sin_port=htons(atoi(argv[2]));
	connect(sock, (SOCKADDR*)&servAdr, sizeof(servAdr));//UDP소켓에 서버 주소 연결 
	
	while(1){
		fputs("Insert message(q to quit): ", stdout);
		fgets(message, sizeof(message), stdin);
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
			
		send(sock, message, strlen(message), 0);//UDP이지만 이미 소켓과 서버가 연결되어있기에 send로 전송. 
		strLen=recv(sock, message, sizeof(message)-1, 0);//recv로 소켓에서 수신한 메시지 get 
		message[strLen]=0;
		printf("Message from server: %s", message);
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
