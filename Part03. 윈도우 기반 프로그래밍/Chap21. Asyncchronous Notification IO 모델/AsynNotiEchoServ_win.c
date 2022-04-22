#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 100

void CompressSockets(SOCKET hSockArr[], int idx, int total);
void CompressEvents(WSAEVENT hEventArr[], int idx, int total);
void ErrorHandling(char * msg);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	
	SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];//���ϵ��� ������ �迭 (���� 64) 
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];//���ϵ鿡 �����Ǵ� �̺�Ʈ�� ������ �迭 
	WSAEVENT newEvent;//������ �̺�Ʈ�� ���� �ӽ����� ���� 
	WSANETWORKEVENTS netEvents;//� ������ �̺�Ʈ������ ��� Ȯ���� ����ü. ������ �����ڵ�� �����Ǿ��ִ�. (line 63)
	
	int numOfClntSock=0;
	int strLen, i;
	int posInfo, startIdx;
	int clntAdrLen;
	char msg[BUF_SIZE];
	
	if(argc!=2){
		printf("Usage: %s <port> \n", argv[0]);
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
		
	newEvent=WSACreateEvent();//������ Event�� ���� �� ����� CreateEvent�Լ��� �ٸ��� ������ �񵿱� �˸�IO���� ������ ���̱⿡ non-signal�� manual��常 �ʿ��ϴ�. �ش� event�� �����. 
	if(WSAEventSelect(hServSock, newEvent, FD_ACCEPT)==SOCKET_ERROR)//���� ���������� ����������μ�, newEvent�� ���� FD_ACCEPT�� Ȯ���Ѵ�. (to os)
		ErrorHandling("WSAEventSelect() error");
		
	hSockArr[numOfClntSock]=hServSock;//WSAEventSelect�Լ�ȣ���� ���Ͽ� ����� ���ϵ�� Event������Ʈ�� �ڵ������� �����ϱ� �����̴�. 
	hEventArr[numOfClntSock]=newEvent;//�̸� ���� ��ġ(index)�� ���Ͻ�Ų��. 
	numOfClntSock++;//�������Ͽ��� �ʿ��� ACCEPT�̺�Ʈ�� ������ �迭�� �����Ѵ�. 
	
	while(1){
		posInfo=WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);//�̺�Ʈ�� �߻��Ҷ����� wait�Ѵ�. 
		startIdx=posInfo-WSA_WAIT_EVENT_0;//�̺�Ʈ �߻���, �̺�Ʈ�� �߻��� ù��° index�� �����´�. 
		
		for(i=startIdx; i<numOfClntSock; i++){//ù index�� ��������, ��� ClntSock�� ��ȸ�Ѵ�.(���� �̺�Ʈ �߻����� �ƽ��Ե� �� �� ���⿡..) 
			int sigEventIdx=WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);//����i�� ���� �̺�Ʈ�� �߻��߳���? timeout���� �񵿱��� üũ.�߻��� sigEvnetIdx�� ���� i��. 
			if((sigEventIdx==WSA_WAIT_FAILED || sigEventIdx==WSA_WAIT_TIMEOUT))//������ üũ�� �������� �̺�Ʈ�� �߻����� �ʾ����� continue�� ���� iŽ�� 
				continue;
			else{//�� ���� i���� �̺�Ʈ�� �߻��߽��ϴ�! 
				sigEventIdx=i;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);//���ϰ� �̺�Ʈ������ ���� � ������ �̺�Ʈ�� �߻��ߴ��� netEvents�� �������ּ��� 
				if(netEvents.lNetworkEvents & FD_ACCEPT){//Accept�̺�Ʈ�� �߻��ߴٸ� 
					if(netEvents.iErrorCode[FD_ACCEPT_BIT]!=0){//������ ���� Accept�� ������Ʈ�� Ȱ��ȭ�Ǿ��ִٸ�(����ó��) 
						puts("Accept Error");
						break;
					}
					clntAdrLen=sizeof(clntAdr);//ACCEPT������ ������ accept����. 
					hClntSock=accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &clntAdrLen);
					newEvent=WSACreateEvent();//�� �� ���ο� Event������Ʈ�� �����ϰ�(���������� manual, non-signal) 
					WSAEventSelect(hClntSock, newEvent, FD_READ|FD_CLOSE);//accept�� ������ Ŭ���̾�Ʈ ������ R/C�̺�Ʈ�� �߻����θ� ������ ������ �̺�Ʈ�� �����Ͽ� ����. 
					
					hEventArr[numOfClntSock]=newEvent;//�׸��� �̵��� Array�� �߰�. 
					hSockArr[numOfClntSock]=hClntSock;
					numOfClntSock++;
					puts("connected new client...");
				}
				
				if(netEvents.lNetworkEvents & FD_READ){//�б� �̺�Ʈ�� �߻��ߴٸ� 
					if(netEvents.iErrorCode[FD_READ_BIT]!=0){
						puts("Read Error");
						break;
					}
					strLen=recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);//�ش� ���Ͽ��� �޽����� �о� msg�� ������ �� 
					send(hSockArr[sigEventIdx], msg, strLen, 0);//������(echo)�Ѵ�. 
				}
				
				if(netEvents.lNetworkEvents & FD_CLOSE){//�ݱ� �̺�Ʈ�� �߻��ߴٸ� (����� ���Ͽ�) 
					if(netEvents.iErrorCode[FD_CLOSE_BIT]!=0){
						puts("Close Error");
						break;
					}
					WSACloseEvent(hEventArr[sigEventIdx]);//�̺�Ʈ ������Ʈ�� �����ϰ� 
					closesocket(hSockArr[sigEventIdx]);//�ش��ϴ� ������ �ݴ´�. 
					
					numOfClntSock--;//Arr����. 
					CompressSockets(hSockArr, sigEventIdx, numOfClntSock);
					CompressEvents(hEventArr, sigEventIdx, numOfClntSock);
				}
			}
		}
	}
	WSACleanup();
	return 0;
}

void CompressSockets(SOCKET hSockArr[], int idx, int total){
	int i;
	for(i=0; i<total; i++)
		hSockArr[i]=hSockArr[i+1];
}

void CompressEvents(WSAEVENT hEventArr[], int idx, int total){
	int i;
	for(i=idx; i<total; i++)
		hEventArr[i]=hEventArr[i+1];
}

void ErrorHandling(char *msg){
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
