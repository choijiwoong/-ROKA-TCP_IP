//void CALLBACK CompletionROUTINE(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
//오류정보(정상0), 완료된 입출력 크기정보, WSASend & WSARecv에 전달된 WSAOVERLAPPED구조체 주소, 특성정보(or 0) 
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void CALLBACK CompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);//수신,전송등이 완료되면 호출. LPWSAOVERLAPPED즉, WSAOVERLAPPED 구조체의 주소는 꼭 전달해야한다. 이벤트는 따로 생성할 필요없이 그대로 전달한다. 
void ErrorHandling(char *message);

WSABUF dataBuf;
char buf[BUF_SIZE];
int recvBytes=0;

int main(int argc, char * argv[]){//수신(서버) 
	WSADATA wsaData;
	SOCKET hLisnSock, hRecvSock;
	SOCKADDR_IN lisnAdr, recvAdr;
	
	WSAOVERLAPPED overlapped;
	WSAEVENT evObj;
	
	int idx, recvAdrSz, flags=0;
	if(argc!=2){
		printf("Usage: %s <port> \n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hLisnSock=WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);//마찬가지로 OVERLAPPED옵션과 함께 소켓 생성 
	memset(&lisnAdr, 0, sizeof(lisnAdr));
	lisnAdr.sin_family=AF_INET;
	lisnAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	lisnAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hLisnSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
		
	recvAdrSz=sizeof(recvAdr);
	hRecvSock=accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);//연결 
	if(hRecvSock==INVALID_SOCKET)
		ErrorHandling("accept() error");
		
	memset(&overlapped, 0, sizeof(overlapped));//WSAOVERLAPPED구조체 초기화 
	dataBuf.len=BUF_SIZE;//WSABUF 세팅 
	dataBuf.buf=buf;
	evObj=WSACreateEvent();//이벤트 오브젝트 생성 manual-mode, non-signal 
	
	if(WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, CompRoutine)==SOCKET_ERROR){//WSARecv시에 CALLBACK으로 사용할 CompRoutine함수 전달. 
		if(WSAGetLastError()==WSA_IO_PENDING)//아직 전송중이라면 
			puts("Background data receive");
	}
	
	idx=WSAWaitForMultipleEvents(1, &evObj, FALSE, WSA_INFINITE, TRUE);//이벤트가 발생할 때까지 기다리는데, 마지막 인자 TRUE로 인해 alertable wait상태로 기다린다. 
	//근데 위 함수 호출 즉 alertable state로 두기위하여 쓸데없ㅇ는 evObj를 생성했다. 이는 dummy object로 이를 최적화하고싶다면 SleepEx와 같은 다른 함수를 사용할 수 있다.(다만 덜 직관적일듯) 
	//총 2가지로 OverlappedIO에서의 입출력 확인이 가능한데, 우리는 Event오브젝트 없이 Completion Routine을 사용하려하지만, 그래도 함수의 인자는 충족시켜야하기에 dummy event를 생성한 것이다. 
	if(idx==WAIT_IO_COMPLETION)//IO가 정상적으로 완료되었다면 
		puts("Overlapped I/O Completed");
	else
		ErrorHandling("WSARecv() error");
		
	WSACloseEvent(evObj);
	closesocket(hRecvSock);
	closesocket(hLisnSock);
	WSACleanup();
	return 0;
}//alertable wait상태로 만드는 함수는 WaitForSingleObjectEx, WaitForMultipleObjectsEx, WSAWaitForMultipleEvents, SleepEx가 있다. 

void CALLBACK CompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags){
	if(dwError!=0){//만약 호출 과정 혹은 이전에 에러가 발생했다면 (예외처리) 
		ErrorHandling("CompRoutine error");
	} else{
		recvBytes=szRecvBytes;//바이트를 수동으로 처리해주고 
		printf("Received message: %d\n", buf);//수신한 메시지 출력 
	}
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
