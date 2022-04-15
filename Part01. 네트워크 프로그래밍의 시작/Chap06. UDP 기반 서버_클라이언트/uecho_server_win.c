#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#define BUF_SIZE 30
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET servSock;
	char message[BUF_SIZE];
	int strLen;
	int clntAdrSz;
	
	SOCKADDR_IN servAdr, clntAdr;
	if(argc!=2){
		printf("Usage: %s <port> \n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	servSock=socket(PF_INET, SOCK_DGRAM, 0);
	if(servSock==INVALID_SOCKET)
		ErrorHandling("UDP socket creation error");
		
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(servSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)//bind socket을 컴퓨터기본IP, 인자로 얻은 포트 
		ErrorHandling("bind() error");
	
	while(1){
		clntAdrSz=sizeof(clntAdr);
		strLen=recvfrom(servSock, message, BUF_SIZE, 0, (SOCKADDR*)&clntAdr, &clntAdrSz);//발신인을 clntAdr에 저장, servSock으로 들어온 정보 message에 저장 
		sendto(servSock, message, strLen, 0, (SOCKADDR*)&clntAdr, sizeof(clntAdr));//받은 메시지를 clntAdr에 전송 
	}
	closesocket(servSock);
	WSACleanup();
	return 0;
} 
