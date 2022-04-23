#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void CALLBACK ReadCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void CALLBACK WriteCompRoutine(DWORD, DWORD, LPWSAOVERLAPPED, DWORD);
void ErrorHandling(char *message);

typedef struct{
	SOCKET hClntSock;//클라이언트 소켓을 담을 핸들 
	char buf[BUF_SIZE];//버퍼 
	WSABUF wsaBuf;//WSABUF
}PER_IO_DATA, *LPPER_IO_DATA;

int main(int argc, char* argv[]){
	WSADATA wsaData;
	SOCKET hLisnSock, hRecvSock;
	SOCKADDR_IN lisnAdr, recvAdr;
	LPWSAOVERLAPPED lpOvLp;
	DWORD recvBytes;
	LPPER_IO_DATA hbInfo;//새로 정의한 구조체 
	int mode=1, recvAdrSz, flagInfo=0;
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup()error!");
		
	hLisnSock=WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	ioctlsocket(hLisnSock, FIONBIO, &mode);//non-blocking 모드(mode값이 0이면 블라킹)로 소켓의 IO를 변경한다. 
	
	memset(&lisnAdr, 0, sizeof(lisnAdr));
	lisnAdr.sin_family=AF_INET;
	lisnAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	lisnAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hLisnSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
		
	recvAdrSz=sizeof(recvAdr);
	while(1){
		SleepEx(100, TRUE);//***alertable wait상태로의 진입을 위한 잦은 호출은 성능의 단점을 야기한다. 이러한 단점은 accept와 입출력을 별도의 쓰레드로 처리하여 해결이 가능한데, 이것이 IOCP이다.*** 
		hRecvSock=accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);//현재 hLisnSock을 non-blocking으로 생성했기에 accept로 생성되는 소켓도 onn-blocking소켓이다. 
		if(hRecvSock==INVALID_SOCKET){//클라이언트 요청없이 accept시도시 반환하는 매크로 상수 
			if(WSAGetLastError()==WSAEWOULDBLOCK)//실제로 오류 유형이 위에 해당되는지 확인 
				continue;
			else
				ErrorHandling("accept() error");
		}
		puts("Client connected.....");
		
		lpOvLp=(LPWSAOVERLAPPED)malloc(sizeof(WSAOVERLAPPED));//WSARecv호출시 전달하는 OVERLAPPED구조체는 소켓마다 각각 있어야 하기에 동적할당 
		memset(lpOvLp, 0, sizeof(WSAOVERLAPPED));
		
		hbInfo=(LPPER_IO_DATA)malloc(sizeof(PER_IO_DATA));//정의한 구조체를 동적할당(포인트로) 
		hbInfo->hClntSock=(DWORD)hRecvSock;//소켓할당 
		(hbInfo->wsaBuf).buf=hbInfo->buf;//wsaBuf멤버 할당 
		(hbInfo->wsaBuf).len=BUF_SIZE;//소켓은 입출력과정에서 hbInfo안의 WSABUF를 사용할 예정이다. 
		
		lpOvLp->hEvent=(HANDLE)hbInfo;//할당된 구조체를 핸들로 형변환하여 lpOvLp(LPWSAOVERLAPPED)의 이벤트로 등록한다. 
		WSARecv(hRecvSock, &(hbInfo->wsaBuf), 1, &recvBytes, &flagInfo, lpOvLp, ReadCompRoutine);//위의 이벤트를 이용해 필요한 다른 정보를 넣는건데, Completioni Routine기반의 Overlapped IO에서는 Event오브젝트가 불필요하기 때문이다(Routine사용시) 
	}
	closesocket(hRecvSock);
	closesocket(hLisnSock);
	WSACleanup();
	return 0;
}

void CALLBACK ReadCompRoutine(DWORD dwError, DWORD szRecvBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags){//이동수단 느낌인가 
	LPPER_IO_DATA hbInfo=(LPPER_IO_DATA)(lpOverlapped->hEvent);//이벤트를 다시 정의한 구조체로 담는다. 
	SOCKET hSock=hbInfo->hClntSock;//클라이언트 소켓 저장 
	LPWSABUF bufInfo=&(hbInfo->wsaBuf);//WSABUF저장 
	DWORD sentBytes;
	
	if(szRecvBytes==0){
		closesocket(hSock);
		free(lpOverlapped->hEvent);//main함수에서 동적할당한 lpOvLp.hEvent로 등록된 정의 구조체와, WSAOVERLAPPED자체의 lpOvLp를 해제 
		free(lpOverlapped);
		puts("Client disconnected.....");
	} else{//수신한 데이터가 있다면 
		bufInfo->len=szRecvBytes;//WSABUF의 길이에 저장한 뒤 
		WSASend(hSock, bufInfo, 1, &sentBytes, 0, lpOverlapped, WriteCompRoutine);//그만큼을 hSock로 전송한다. echo! 
	}//WSASend를 이용하여 재전송 한 뒤, 전송완료시 WriteCompRoutine을 호출하는데, 이는 다음 echo를 위하여 ReadCompRoutine과 함께 Recv를 호출한다. 
}

void CALLBACK WriteCompRoutine(DWORD dwError, DWORD szSendBytes, LPWSAOVERLAPPED lpOverlapped, DWORD flags){
	LPPER_IO_DATA hbInfo=(LPPER_IO_DATA)(lpOverlapped->hEvent);
	SOCKET hSock=hbInfo->hClntSock;
	LPWSABUF bufInfo=&(hbInfo->wsaBuf);//입출력시 이거사용 
	DWORD recvBytes;
	int flagInfo=0;
	WSARecv(hSock, bufInfo, 1, &recvBytes, &flagInfo, lpOverlapped, ReadCompRoutine);//소켓으로부터 수신한다. 
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
