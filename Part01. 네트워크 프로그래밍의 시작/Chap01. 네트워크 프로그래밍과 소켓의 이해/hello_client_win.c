#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char* message);

int main(int argc, char* argv[]){
	WSADATA wsaData;//윈속 버전에 맞는 초기화를 위함 
	SOCKET hSocket;
	SOCKADDR_IN servAddr;
	
	char message[30];
	int strLen;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)//윈속 초기화 
		ErrorHandling("WSAStartup() error!");
	
	hSocket=socket(PF_INET, SOCK_STREAM, 0);//소켓 생성 
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("socket() error!");
	
	memset(&servAddr, 0, sizeof(servAddr));//서버주소 0초기화 
	servAddr.sin_family=AF_INET;//속성 설정 
	servAddr.sin_addr.s_addr=inet_addr(argv[1]);//address
	servAddr.sin_port=htons(atoi(argv[2]));//port
	
	if(connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)//클라이언트 소켓을 서버에 연결요청 
		ErrorHandling("connect() error!");
		
	strLen=recv(hSocket, message, sizeof(message)-1, 0);//receive. 성공 시 수신한 바이트 수(EOF전송시 0) 
	if(strLen==-1)
		ErrorHandling("read() error!");
	printf("Message from server: %s\n", message);
	
	closesocket(hSocket);
	WSACleanup();
	
	return 0;
}

void ErrorHandling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
