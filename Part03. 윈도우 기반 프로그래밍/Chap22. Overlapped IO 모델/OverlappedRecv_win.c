#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char* argv[]){//����(����) 
	WSADATA wsaData;
	SOCKET hLisnSock, hRecvSock;
	SOCKADDR_IN lisnAdr, recvAdr;
	int recvAdrSz;
	
	WSABUF dataBuf;//WSARecv, WSASend���� ����ϱ� ���� WSABUF 
	WSAEVENT evObj;//WSAOVERLAPPED.hEvent�� ����� �̺�Ʈ ������Ʈ 
	WSAOVERLAPPED overlapped;//OVERLAPPED IO�� �ʿ��� ����ü 
	
	char buf[BUF_SIZE];
	int recvBytes=0, flags=0;//������ �������� ����Ʈ�� ����� ���� 
	if(argc!=2){
		printf("Usage: %s <port> \n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hLisnSock=WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);//WSASocket�� OVERLAPPED�÷��׿� �Բ� ���� 
	memset(&lisnAdr, 0, sizeof(lisnAdr));//���� �ּ� �ʱ�ȭ 
	lisnAdr.sin_family=AF_INET;
	lisnAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	lisnAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hLisnSock, (SOCKADDR*)&lisnAdr, sizeof(lisnAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hLisnSock, 5)==SOCKET_ERROR)
		ErrorHandling("listen() error");
		
	recvAdrSz=sizeof(recvAdr);
	hRecvSock=accept(hLisnSock, (SOCKADDR*)&recvAdr, &recvAdrSz);//Ŭ���̾�Ʈ ������ �ް� �ϴ� ������ �Ϲ����� ���� ������ ���� 
	
	evObj=WSACreateEvent();//manual-mode, non-signaled�� Event������Ʈ ���� 
	memset(&overlapped, 0, sizeof(overlapped));//WSAOVERLAPPED�ʱ�ȭ 
	overlapped.hEvent=evObj;//�� �̺�Ʈ �Ҵ� 
	dataBuf.len=BUF_SIZE;//WSABUF���Ŀ� �°� ������ ���� for WSARecv 
	dataBuf.buf=buf;
	
	if(WSARecv(hRecvSock, &dataBuf, 1, &recvBytes, &flags, &overlapped, NULL)==SOCK_ERROR){//1���� WSABUF�� ������ ��. SOCK_ERROR�� �� �ٷ� ��ȯ�� ���� ���� ���(�����Ͱ� ���� ���� �ʾƼ���..) 
		if(WSAGetLastError()==WSA_IO_PENDING){//���� ���� ������ �������̶�� 
			puts("Background data receive");
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);//�̺�Ʈ�� Ȱ��ȭ�� �� ���� ��ٸ���. 
			WSAGetOverlappedResult(hRecvSock, &overlapped, &recvBytes, FALSE, NULL);//����� ��´�..? �Ƹ� recvBytes�� 
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
