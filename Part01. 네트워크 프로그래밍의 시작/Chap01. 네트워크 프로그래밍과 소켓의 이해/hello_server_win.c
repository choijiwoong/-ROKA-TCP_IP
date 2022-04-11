#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char* message);

int main(int argc, char* argv[]){
	WSADATA wsaData;//윈도우 소켓의 버전을 알리고, 버전을 지원하는 라이브러리의 초기화 작업을 위함. 
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;
	
	int szClntAddr;
	char message[]="Hello World!";
	if(argc!=2){
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)//부버전, 주버전 정보와 WSADATA구조체 주소를 넘겨주어 초기화. 
		ErrorHandling("WSAStartup() error!");
		
	hServSock=socket(PF_INET, SOCK_STREAM, 0);//server socket의 handle 
	if(hServSock==INVALID_SOCKET)
		ErrorHandling("socket() error");
	
	memset(&servAddr, 0, sizeof(servAddr));//서버주소 0초기화 
	servAddr.sin_family=AF_INET;//속성 설정 
	servAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAddr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))==SOCKET_ERROR)//server socket과 socket address를 binding. IP와 Port할당 
		ErrorHandling("bind() error");
		
	if(listen(hServSock, 5)==SOCKET_ERROR)//통신대기 
		ErrorHandling("listen() error");
		
	szClntAddr=sizeof(clntAddr);
	hClntSock=accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);//클라이언트의 연결요청 수락을 위한 accept 
	if(hClntSock==INVALID_SOCKET)
		ErrorHandling("accept() error");
		
	send(hClntSock, message, sizeof(message), 0);//성공시 전송된 바이트 수 리턴. 실패시 SOCKET_ERROR(소켓핸들, 버퍼주소, 바이트수, 옵션) 
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();//초기화한 소켓 라이브러리 해제 
	
	return 0;
}

void ErrorHandling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
