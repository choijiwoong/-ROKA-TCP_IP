#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char *argv[]){//ch4의 echo_client_win.c과 동작 
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAdr, clntAdr;
	TIMEVAL timeout;
	fd_set reads, cpyReads;
	
	int adrSz;
	int strLen, fdNum, i;
	char buf[BUF_SIZE];
	
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
		
	FD_ZERO(&reads);
	FD_SET(hServSock, &reads);//서버꺼 1
	
	while(1){
		cpyReads=reads;//fd_set복사 
		timeout.tv_sec=5;
		timeout.tv_usec=5000;
		
		if((fdNum=select(0, &cpyReads, 0, 0, &timeout))==SOCKET_ERROR)//fd_set에 대해 select수행.  
			break;
		
		if(fdNum==0)//timeout
			continue;
			
		for(i=0; i<reads.fd_count; i++){//fd_set의 모든 파일디스크립터에 대하여(정확히는 핸들) 
			if(FD_ISSET(reads.fd_array[i], &cpyReads)){//실제 변화가 있는지 확인하고
				if(reads.fd_array[i]==hServSOck){//서버핸들 변화의 경우 클라이언트 요청으로 간주. 
					adrSz=sizeof(clntAdr);
					hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
					FD_SET(hClntSock, &reads);//클라이언트 추가 및 세팅 
					printf("connected client: %d\n", hClntSock);
				} else{//나머지는 클라이언트 변화로 간주 
					strLen=recv(reads.fd_array[i], buf, BUF_SIZE-1, 0);//해당 소켓으로부터 수신한 데이터를 buf에 저장 
					if(strLen==0){//eof일시  
						FD_CLR(reads.fd_array[i], buf, BUF_SIZE-1, 0);//unset후 소켓 close 
						closesocket(cpyReads.fd_array[i]);
						printf("closed client: %d\n", cpyReads.fd_array[i]);
					} else{
						send(reads.fd_array[i], buf, strLen, 0);//수신받은거 그대로 전송
					}
				}
			}
		}
	} 
	closesocket(hServSock);
	WSACleanup();
	return 0;
}

void ErrorHandling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
