#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
void ErrorHandling(char *message);
void ShowSocketBufSize(SOCKET sock);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hSock;
	int sndBuf, rcvBuf, state;//입출력 버퍼의 옵션 변경에 사용할 변수, 참조에 사용할 변수 
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
	
	hSock=socket(PF_INET, SOCK_STREAM, 0);//TCP로 기본 생성 
	ShowSocketBufSize(hSock);//getopt를 통해 default 입출력버퍼의 크기 출력. 
	
	sndBuf=1024*3, rcvBuf=1024*3;//변경하고자하는 입출력버퍼의사이즈 
	state=setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, sizeof(rcvBuf));//setsockopt로 SO_RCVBUF를 (char*)로 변경. (리눅스와의 차이점. 리눅스는 void*로 설정) 
	if(state==SOCKET_ERROR)
		ErrorHandling("setsockopt() error!");
		
	state=setsockopt(hSock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, sizeof(sndBuf));
	if(state==SOCKET_ERROR)
		ErrorHandling("setsockopt() error!");
		
	ShowSocketBufSize(hSock);//각각 sndBuf와 rcvBuf크기만큼 SO_RCVBUF와 SO_SNDBUF변경 후 현제 소켓의 버퍼사이즈 출력. 
	closesocket(hSock);
	WSACleanup();
	return 0;
} 

void ShowSocketBufSize(SOCKET sock){
	int sndBuf, rcvBuf, state, len;
	
	len=sizeof(sndBuf);
	state=getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&sndBuf, &len);
	if(state==SOCKET_ERROR)
		ErrorHandling("getsockopt() error");
		
	len=sizeof(rcvBuf);
	state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&rcvBuf, &len);
	if(state==SOCKET_ERROR)
		ErrorHandling("getsockopt() error");
		
	printf("Input buffer size: %d\n", rcvBuf);
	printf("Output buffer size: %d\n", sndBuf);
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
