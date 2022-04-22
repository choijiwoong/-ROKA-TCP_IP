/*
SOCKET WSASocket(int af, int type, int protocol, LPWSAPROTOCOL_INFO lpProtocolInfo, GROUP g, DWORD dwFlags);
params�� �������� ü�� ����, ���� ������ ���۹��, �� ���ϻ��� ��������, ������ Ư������(null), �����(0), ������ �Ӽ�����(WSA_FLAG_OVERLAPPED)�� ���� 

int WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesSent, DWORD dwFlags, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTUNE lpCompletionRoutine);
�����ڵ�, WSABUF�迭, ����, ���۹���Ʈ�����庯���ּ�, ����������Ư��, WSAOVERLAPPED����ü �ּ�(����Ȯ��), �Լ��ּҰ�(����Ȯ��) 

typedef struct _WSAOVERLAPPED{
	DWORD Internal;//OS��������� 
	DWORD InternalHigh;//OS��������� 
	DWORD Offset;//����� 
	DWORD OffsetHigh;//����� 
	WSAEVENT hEvent;//�길 ������. 
} WSAOVERLAPPED, *LPWSAOVERLAPPED;

BOOL WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped, LPDWORD lpcbTransfer, BOOl fWait, LPDWORD lpdwFlags);
Overlapped IO�� ����� ������ �ڵ�, WSAOVERLAPPED����ü �ּ�, ����Ʈũ�� ������ ��, wait����, �μ����� ���� ����

int WSARecv(~) 

int WSAGetLastError(void);//������Ȳ�� ���� ���� ��(������ ������ �˸��� ��) ��ȯ WSA_IO_PENDING(������)�� �����Ͱ� ���۵ǰ� ������ ����. 
*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
void ErrorHandling(char *msg);

int main(int argc, char *argv[]){//Ŭ���̾�Ʈ(����) 
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN sendAdr;
	
	WSABUF dataBuf;
	char msg[]="Network is Computer!";//������ �޽���. �� WSABUF�� ��ȯ ����. 
	int sendBytes=0;//������ ����Ʈ�� ũ�Ⱑ ��ϵ� ����. 
	
	WSAEVENT evObj;//�̺�Ʈ for WSAOVERLAPPED.hEvent 
	WSAOVERLAPPED overlapped;//OVERLAPPED_IO�� ���� ����ü. ���� WSASend()�� �� �̻��� ���� �����͸� �����ϴ� ��쿡 ���� �غ��ؾ��Ѵ�. 
	
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	hSocket=WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);//�Ϲ����� ���� ������ ���������, �������� overlap�÷��׸� �����Ѵ�. �׷��������� ���ŷ���� �۵��ϴ� �߹����� ������ �����ȴ�. 
	memset(&sendAdr, 0, sizeof(sendAdr));
	sendAdr.sin_family=AF_INET;
	sendAdr.sin_addr.s_addr=inet_addr(argv[1]);
	sendAdr.sin_port=htons(atoi(argv[2]));
	
	if(connect(hSocket, (SOCKADDR*)&sendAdr, sizeof(sendAdr))==SOCKET_ERROR)
		ErrorHandling("connect() error!");
		
	evObj=WSACreateEvent();//�̺�Ʈ ������Ʈ ����(����é�Ϳ��� ������ �Ͱ� ���������� manual-mode, un-signaled �̺�Ʈ ����) 
	memset(&overlapped, 0, sizeof(overlapped));//WSAOVERLAPPED����ü 0���� �ʱ�ȭ 
	overlapped.hEvent=evObj;//�̺�Ʈ �Ҵ� 
	dataBuf.len=strlen(msg)+1;//WSABUF����(WSASend�� WSABUF�� ����Ѵ�_Gatter/Scatter IO) 
	dataBuf.buf=msg;
	
	if(WSASend(hSocket, &dataBuf, 1, &sendBytes, 0, &overlapped, NULL)==SOCK_ERROR){//����, ������ WSABUF�迭, ũ��, ���۹���Ʈ������, ����, WSAOVERLAPPED����ü, �ɼ�) 
		//SOCK_ERROR�� �ﰢ���� ��ȯ�� �ƴ� ������ �������� ���. 
		if(WSAGetLastError()==WSA_TO_PENDING){//�������� ������ ���� �������� ��� 
			puts("Background data send");//��׶��忡�� �����Ͱ� ���۵ǰ������� �˸��� 
			WSAWaitForMultipleEvents(1, &evObj, TRUE, WSA_INFINITE, FALSE);//�̺�Ʈ�� Ȱ��ȭ �� ������ ��ٸ���. 
			WSAGetOverlappedResult(hSocket, &overlapped, &sendBytes, FALSE, NULL);//����� ��´�.? 
		} else{
			ErrorHandling("WSASend() error");
		}
	}//else�� ���� ���� ���� ������ SOCK_ERROR�� ��ȯ���� �ʾҴٸ� �������� 0���μ� �������� ũ�Ⱑ �۾� ȣ��� ���ÿ� ��ȯ�Ǿ� �ﰢ������ sendBytes������ ������ ����Ʈ���� ����Ǿ��ִ�. 
	
	printf("Send data size: %d\n", sendBytes);
	WSACloseEvent(evObj);
	closesocket(hSocket);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
