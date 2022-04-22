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
	
	SOCKET hSockArr[WSA_MAXIMUM_WAIT_EVENTS];//소켓들을 저장할 배열 (보통 64) 
	WSAEVENT hEventArr[WSA_MAXIMUM_WAIT_EVENTS];//소켓들에 대응되는 이벤트를 저장할 배열 
	WSAEVENT newEvent;//서버의 이벤트를 담을 임시적인 변수 
	WSANETWORKEVENTS netEvents;//어떤 종류의 이벤트인지를 담아 확인할 구조체. 종류와 에러코드로 구성되어있다. (line 63)
	
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
		
	newEvent=WSACreateEvent();//이전에 Event를 만들 때 사용한 CreateEvent함수와 다르게 지금은 비동기 알림IO모델을 구현할 것이기에 non-signal의 manual모드만 필요하다. 해당 event를 만든다. 
	if(WSAEventSelect(hServSock, newEvent, FD_ACCEPT)==SOCKET_ERROR)//현재 서버소켓을 관찰대상으로서, newEvent를 통해 FD_ACCEPT를 확인한다. (to os)
		ErrorHandling("WSAEventSelect() error");
		
	hSockArr[numOfClntSock]=hServSock;//WSAEventSelect함수호출을 통하여 연결될 소켓들과 Event오브젝트의 핸들정보를 전달하기 위함이다. 
	hEventArr[numOfClntSock]=newEvent;//이를 위해 위치(index)를 통일시킨다. 
	numOfClntSock++;//서버소켓에서 필요한 ACCEPT이벤트를 각각의 배열에 갱신한다. 
	
	while(1){
		posInfo=WSAWaitForMultipleEvents(numOfClntSock, hEventArr, FALSE, WSA_INFINITE, FALSE);//이벤트가 발생할때까지 wait한다. 
		startIdx=posInfo-WSA_WAIT_EVENT_0;//이벤트 발생시, 이벤트가 발생한 첫번째 index를 가져온다. 
		
		for(i=startIdx; i<numOfClntSock; i++){//첫 index를 시작으로, 모든 ClntSock를 순회한다.(다음 이벤트 발생지를 아쉽게도 알 수 없기에..) 
			int sigEventIdx=WSAWaitForMultipleEvents(1, &hEventArr[i], TRUE, 0, FALSE);//현재i는 정말 이벤트가 발생했나요? timeout없이 비동기적 체크.발생시 sigEvnetIdx에 현재 i값. 
			if((sigEventIdx==WSA_WAIT_FAILED || sigEventIdx==WSA_WAIT_TIMEOUT))//위에서 체크한 시점에서 이벤트가 발생하지 않았으면 continue로 다음 i탐색 
				continue;
			else{//네 현재 i에서 이벤트가 발생했습니다! 
				sigEventIdx=i;
				WSAEnumNetworkEvents(hSockArr[sigEventIdx], hEventArr[sigEventIdx], &netEvents);//소켓과 이벤트종류를 토대로 어떤 종류의 이벤트가 발생했는지 netEvents에 저장해주세요 
				if(netEvents.lNetworkEvents & FD_ACCEPT){//Accept이벤트가 발생했다면 
					if(netEvents.iErrorCode[FD_ACCEPT_BIT]!=0){//하지만 만약 Accept의 에러비트가 활성화되어있다면(예외처리) 
						puts("Accept Error");
						break;
					}
					clntAdrLen=sizeof(clntAdr);//ACCEPT에러가 없을시 accept진행. 
					hClntSock=accept(hSockArr[sigEventIdx], (SOCKADDR*)&clntAdr, &clntAdrLen);
					newEvent=WSACreateEvent();//그 후 새로운 Event오브젝트를 생성하고(마찬가지로 manual, non-signal) 
					WSAEventSelect(hClntSock, newEvent, FD_READ|FD_CLOSE);//accept로 생성한 클라이언트 소켓의 R/C이벤트를 발생여부를 새로이 생성한 이벤트에 저장하여 관찰. 
					
					hEventArr[numOfClntSock]=newEvent;//그리고 이들을 Array에 추가. 
					hSockArr[numOfClntSock]=hClntSock;
					numOfClntSock++;
					puts("connected new client...");
				}
				
				if(netEvents.lNetworkEvents & FD_READ){//읽기 이벤트가 발생했다면 
					if(netEvents.iErrorCode[FD_READ_BIT]!=0){
						puts("Read Error");
						break;
					}
					strLen=recv(hSockArr[sigEventIdx], msg, sizeof(msg), 0);//해당 소켓에서 메시지를 읽어 msg에 저장한 뒤 
					send(hSockArr[sigEventIdx], msg, strLen, 0);//재전송(echo)한다. 
				}
				
				if(netEvents.lNetworkEvents & FD_CLOSE){//닫기 이벤트가 발생했다면 (저장된 소켓에) 
					if(netEvents.iErrorCode[FD_CLOSE_BIT]!=0){
						puts("Close Error");
						break;
					}
					WSACloseEvent(hEventArr[sigEventIdx]);//이벤트 오브젝트를 해제하고 
					closesocket(hSockArr[sigEventIdx]);//해당하는 소켓을 닫는다. 
					
					numOfClntSock--;//Arr갱신. 
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
