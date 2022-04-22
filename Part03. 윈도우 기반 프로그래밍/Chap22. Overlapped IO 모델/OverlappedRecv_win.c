#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char* argv[]){//서버(수신) 
	WSADATA wsaData;
	SOCKET hLisnSock, hRecvSock;
	SOCKADDR_IN lisnAdr, recvAdr;
	int recvAdrSz;
	
	WSABUF dataBuf;//WSARecv, WSASend등을 사용하기 위한 WSABUF 
	WSAEVENT evObj;//WSAOVERLAPPED.hEvent에 저장될 이벤트 오브젝트 
	WSAOVERLAPPED overlapped;//OVERLAPPED IO에 필요한 구조체 
	
	char buf[BUF_SIZE];
	int recvBytes=0, flags=0;//수신한 데이터의 바이트가 저장될 변수 
	if(argc!=2){
		printf("Usage: %s <port> \n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hLisnSock=WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);//WSASocket을 OVERLAPPED플래그와 함께 생성 
	memset(&lisnAdr, 0, sizeof(lisnAdr));//소켓 주소 초기화 
	lisnAdr.sin_family=AF_INET;
	lisnAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	lisnAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hLisnSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
		
	recvAdrSz=sizeof(recvAdr);
	hRecvSock=accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);//클라이언트 접속을 받고 하는 과정은 일반적인 소켓 과정과 동일 
	
	evObj=WSACreateEvent();//manual-mode, non-signaled의 Event오브젝트 생성 
	memset(&overlapped, 0, sizeof(overlapped));//WSAOVERLAPPED초기화 
	overlapped.hEvent=evObj;//및 이벤트 할당 
	dataBuf.len=BUF_SIZE;//WSABUF형식에 맞게 데이터 저장 for WSARecv 
	dataBuf.buf=buf;
	
	if(WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, NULL)==SOCK_ERROR){//1개의 WSABUF를 수신할 것. SOCK_ERROR일 시 바로 반환이 되지 않은 경우(데이터가 아주 작진 않아서리..) 
		if(WSAGetLastError()==WSA_IO_PENDING){//만약 아직 수신이 진행중이라면 
			puts("Background data receive");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);//이벤트가 활성화될 때 까지 기다린다. 
			WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);//결과를 얻는다..? 아마 recvBytes에 
		} else{
			ErrorHandling("WSARecv() error");
		}
	}
	
	printf("Received message: %d\n", buf);
	WSACleanEvent(evObj);
	closesocket(hRecvSock);
	closesocket(hLisnSock);
	WSACleanup();
	
	return 0;
} 

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
