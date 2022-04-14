#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4//result size for receive
#define OPSZ 4//Num하나의 바이트사이즈 
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSocket;
	char opmsg[BUF_SIZE];
	int result, opndCnt, i;
	SOCKADDR_IN servAdr;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)//WinSoc Start
		ErrorHandling("WSAStartup() error!");
		
	hSocket=socket(PF_INET, SOCK_STREAM, 0);
	if(hSocket==INVALID_SOCKET)
		ErrorHandling("socket() error");
		
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr(argv[1]);
	servAdr.sin_port=htons(atoi(argv[2]));
	
	if(connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)//make socket by default setting & connect to server address
		ErrorHandling("connect() error!");
	else
		puts("Connected.........");
		
	fputs("Operand count: ", stdout);
	scanf("%d", &opndCnt);//get operand count
	opmsg[0]=(char)opndCnt;//save as char
	
	for(i=0; i<opndCnt; i++){
		printf("Operand %d: ", i+1);
		scanf("%d", (int*)&opmsg[i*OPSZ+1]);//save as int(4bytes)
	}
	fgetc(stdin);//fflush
	fputs("Operator: ", stdout);
	scanf("%c", &opmsg[opndCnt*OPSZ+1]);//get operator a char to end of opmsg
	send(hSocket, opmsg, opndCnt*OPSZ+2, 0);//fourth argument is option
	recv(hSocket, &result, RLT_SIZE, 0);//get result as much as RESULT_SIZE
	
	printf("Operation result: %d\n", result);
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
