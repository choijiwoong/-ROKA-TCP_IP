#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUF_SIZE 30
void ErrorHandling(char *message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	SOCKET hAcptSock, hRecvSock;
	
	SOCKADDR_IN recvAdr;
	SOCKADDR_IN sendAdr;
	int sendAdrSize, strLen;
	char buf[BUF_SIZE];
	int result;
	
	fd_set read, except, readCopy, exceptCopy;//Select를 이용하여 SIGURG를 대체하여 사용할 예정. select인자에 사용할 변수들 
	struct timeval timeout;//마찬가지 
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error");
		
	hAcptSock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&recvAdr, 0, sizeof(revcAdr));
	recvAdr.sin_family=AF_INET;
	recvAdr.sin_addr.s_addr=htonl(INADDR_ANY);
	recvAdr.sin_port=htons(atoi(argv[1]));
	
	if(bind(hAcptSock, (SOCKADDR*)&recvAdr, sizeof(recvAdr))==SOCKET_ERROR)
		ErrorHandling("bind() error");
	if(listen(hAcptSock, 5)==SOCKET_ERROR)//가용  
		ErrorHandling("listen() error");
		
	sendAdrSize=sizeof(sendAdr);
	hRecvSock=accpet(hAcptSock, (SOCKADDR*)&sendAdr, &sendAdrSize));//클라이언트로부터 요청 수락. 
	FD_ZERO(&read);//read 초기화 
	FD_ZERO(&except);//except 초기화 
	FD_SET(hRecvSock, &read);//수신소켓만 세팅. 
	FD_SET(hRecvSock, &except);
	
	while(1){
		readCopy=read;//fd_set을 복사한다음 
		exceptCopy=except;//복사한다음 
		timeout.tv_sec=5;
		timeout.tv_usec=0;
		
		result=select(0, &readCopy, 0 &exceptCopy, &timeout);//복사한 fd_set을 기반으로 select호출. 만약 메시지가 수신되거나 except발생시 result가 0이상일것임 
		
		if(result>0){
			if(FD_ISSET(hRecvSock, &exceptCopy)){//만약 except가 발생했다면(SIGURG대용 except) 
				strLen=recv(hRecvSock, buf, BUF_SIZE-1, MSG_OOB);//MSG_OOB로 recv후 출력. 
				buf[strLen]=0;
				printf("Urgent message: %s\n", buf);
			}
			
			if(FD_ISSET(hRecvSock, &readCopy)){//만약 일반적인 메시지 수신이라면 
				strLen=recv(hRecvSock, buf, BUF_SIZE-1, 0);//revd
				if(strLen==0){//EOF시 닫기 
					break;
					closesocket(hRecvSock);
				}else{
					buf[strLen]=0;
					puts(buf);
				}
			}
		}
	}
	
	closesocket(hAcptSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
