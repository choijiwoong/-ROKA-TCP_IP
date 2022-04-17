#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(char *message);

int main(int argc, char *argv[]){//ch4�� echo_client_win.c�� ���� 
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
	FD_SET(hServSock, &reads);//������ 1
	
	while(1){
		cpyReads=reads;//fd_set���� 
		timeout.tv_sec=5;
		timeout.tv_usec=5000;
		
		if((fdNum=select(0, &cpyReads, 0, 0, &timeout))==SOCKET_ERROR)//fd_set�� ���� select����.  
			break;
		
		if(fdNum==0)//timeout
			continue;
			
		for(i=0; i<reads.fd_count; i++){//fd_set�� ��� ���ϵ�ũ���Ϳ� ���Ͽ�(��Ȯ���� �ڵ�) 
			if(FD_ISSET(reads.fd_array[i], &cpyReads)){//���� ��ȭ�� �ִ��� Ȯ���ϰ�
				if(reads.fd_array[i]==hServSOck){//�����ڵ� ��ȭ�� ��� Ŭ���̾�Ʈ ��û���� ����. 
					adrSz=sizeof(clntAdr);
					hClntSock=accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
					FD_SET(hClntSock, &reads);//Ŭ���̾�Ʈ �߰� �� ���� 
					printf("connected client: %d\n", hClntSock);
				} else{//�������� Ŭ���̾�Ʈ ��ȭ�� ���� 
					strLen=recv(reads.fd_array[i], buf, BUF_SIZE-1, 0);//�ش� �������κ��� ������ �����͸� buf�� ���� 
					if(strLen==0){//eof�Ͻ�  
						FD_CLR(reads.fd_array[i], buf, BUF_SIZE-1, 0);//unset�� ���� close 
						closesocket(cpyReads.fd_array[i]);
						printf("closed client: %d\n", cpyReads.fd_array[i]);
					} else{
						send(reads.fd_array[i], buf, strLen, 0);//���Ź����� �״�� ����
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
