#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#defin BUF_SIZE 30
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;
	if(argc!=3){
		printf("Usage: %s <IP> <port> \n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error~!");
		
	hSocket=socket(PF_INET, SOCK_STREAM, 0);
	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family=AF_INT;
	sendAdr.sin_addr.s_addr=inet_addr(argv[1]);
	sendAdr.sin_port=htons(atoi(argv[2]));
	
	if(connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
		
	send(hSocket, "123", 3, 0);
	send(hSocket, "4", 1, MSG_OOB);
	send(hSocket, "567", 3, 0);
	send(hSocket, "890", 3, MSG_OOB);//클라이언트에서는 이전과 동일하게 그냥 send에 세번째 인자로 MSG_OOB긴급옵션 전달
	
	closesocket(hSocket);
	WSACleanup();
	return 0; 
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
