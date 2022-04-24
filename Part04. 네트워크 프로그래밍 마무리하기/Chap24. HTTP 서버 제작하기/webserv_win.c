//HTTP는 상태가 존재하지 않는 Stateless 프로토콜인데, 이를 보완하기 위해 쿠키와 세션을 사용한다.
//요청 메시지는 요청 방식, 메시지 헤더, 공백라인, 메시지 몸체로 구성되며, 응답 메시지는 상태라인, 메시지 헤더, 공백라인, 메시지 몸체로 구성된다. 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <process.h>

#define BUF_SIZE 2048
#define BUF_SMALL 100

unsigned WINAPI RequestHandler(void* arg);
char* ContentType(char* file);
void SendData(SOCKET sock, char* ct, char* filename);
void SendErrorMSG(SOCKET sock);
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	
	HANDLE hThread;
	DWORD dwThreadID;
	int clntAdrSize;
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hServSock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	servAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hServSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
		
	while(1){
		clntAdrSize=sizeof(clntAdr);
		hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &clntAdrSize);//클라이언트 연결 후 별도의 쓰레드로 
		printf("Connection Request: %s:%d\n", inet_ntoa(clntAdr.sin_addr), ntohs(clntAdr.sin_addr.s_addr));
		hThread=(HANDLE)_beginthreadex(NULL, 0, RequestHandler, (void*)hClntSock, 0, (unsigned*)&dwThreadID);//request handler를 클라이언트 소켓과 함께 실행 
	}
	clsoesocket(hServSock);
	WSACleanup();
	return 0;
}

unsigned WINAPI RequestHandler(void * arg){
	SOCKET hClntSock=(SOCKET)arg;
	char buf[BUF_SIZE];
	char method[BUF_SMALL];
	char ct[BUF_SMALL];
	char fileName[BUF_SMALL];
	
	recv(hClntSock, buf, BUF_SIZE, 0);//클라이언트 소켓으로부터 데이터 수신 
	if(strstr(buf, "HTTP/")==NULL){//buf에서 서브문자열 HTTP/를 찾는다.(정상적인 HTTP응답인지) 
		SendErrorMSG(hClntSock);
		closesocket(hClntSock);
		return 1;
	}
	
	strcpy(method, strtok(buf, " /"));//buf를 /단위로 tokenize한 결과를 method에 저장한다. 
	if(strcmp(method, "GET"))//메서드가 GET이 아니라면 except(GET만 구현한다) 
		SendErrorMSG(hClntSock);
		
	strcpy(fileName, strtok(NULL, " /"));//전송에 필요한 파일 이름 
	strcpy(ct, ContentType(fileName));//파일의 타입등을 할당한다음 
	SendData(hClntSock, ct, fileName);//위의 정보를 기반으로 데이터를 전송한다. 
	return 0;
}

void SendData(SOCKET sock, char* ct, char* fileName){
	char protocol[]="HTTP/1.0 200 OK\r\n";//응답을 위함 
	char servName[]="Server:simple web server\r\n";
	char cntLen[]="Conetent-length:2048\r\n";
	char cntType[BUF_SMALL];
	char buf[BUF_SIZE];
	FILE* sendFile;
	
	sprintf(cntType, "Content-type:%s\r\n\r\n", ct);//cntType에 인자로 들어온 content정보를 출력하고 
	if((sendFile=fopen(fileName, "r"))==NULL){//해당 파일을 fopen한 뒤 
		SendErrorMSG(sock);
		return;
	}
	
	//응답 메시지를 client socket에 전송한다. 
	send(sock, protocol, strlen(protocol), 0);//헤더정보 전송. 헤더의 내용으로 프로토콜, 서버이름, 콘텐츠 길이, 콘텐츠 타입을 보낸다 
	send(sock, servName, strlen(servName), 0);
	send(sock, cntLen, strlen(cntLen), 0);
	send(sock, cntType, strlen(cntType), 0);
	
	while(fgets(buf, BUF_SIZE, sendFile)!=NULL)//요청 데이터 전송 per buffer
		send(sock, buf, strlen(buf), 0);
		
	closesocket(sock);
}

void SendErrorMSG(SOCKET sock){
	char protocol[]="HTTP/1.0 400 Bad Request\r\n";
	char servName[]="Server:simple web server\r\n";
	char cntLen[]="Content-length:2048\r\n";
	char cntType[]="Content-type:text/html\r\n\r\n";
	char content[]="<html><head><title>NETWORK</title></head>"
		"<body><font size=+5><br>오류발생! 요청 파일명 및 요청 방식 확인"
		"</font></body></html>";
	
	send(sock, protocol, strlen(protocol), 0);
	send(sock, servName, strlen(servName), 0);
	send(sock, cntLen, strlen(cntLen), 0);
	send(sock, cntType, strlen(cntType), 0);
	send(sock, content, strlen(content), 0);
	closesocket(sock);
}

char* ContentType(char* file){
	char extension[BUF_SMALL];
	char fileName[BUF_SMALL];
	strcpy(fileName, file);
	strtok(fileName, ".");
	strcpy(extension, strtok(NULL, "."));
	if(!strcmp(extension, "html")||!strcmp(extension, "htm"))//extension즉 확장자를 기반으로 콘텐츠 타입을 리턴한다. 
		return "text/html";
	else
		return "text/plain";
}

void ErrorHandling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
