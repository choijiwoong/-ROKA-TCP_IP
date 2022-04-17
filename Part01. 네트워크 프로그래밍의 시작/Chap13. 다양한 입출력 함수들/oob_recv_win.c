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
	
	fd_set read, except, readCopy, exceptCopy;//Select�� �̿��Ͽ� SIGURG�� ��ü�Ͽ� ����� ����. select���ڿ� ����� ������ 
	struct timeval timeout;//�������� 
	
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
	if(listen(hAcptSock, 5)==SOCKET_ERROR)//����  
		ErrorHandling("listen() error");
		
	sendAdrSize=sizeof(sendAdr);
	hRecvSock=accpet(hAcptSock, (SOCKADDR*)&sendAdr, &sendAdrSize));//Ŭ���̾�Ʈ�κ��� ��û ����. 
	FD_ZERO(&read);//read �ʱ�ȭ 
	FD_ZERO(&except);//except �ʱ�ȭ 
	FD_SET(hRecvSock, &read);//���ż��ϸ� ����. 
	FD_SET(hRecvSock, &except);
	
	while(1){
		readCopy=read;//fd_set�� �����Ѵ��� 
		exceptCopy=except;//�����Ѵ��� 
		timeout.tv_sec=5;
		timeout.tv_usec=0;
		
		result=select(0, &readCopy, 0 &exceptCopy, &timeout);//������ fd_set�� ������� selectȣ��. ���� �޽����� ���ŵǰų� except�߻��� result�� 0�̻��ϰ��� 
		
		if(result>0){
			if(FD_ISSET(hRecvSock, &exceptCopy)){//���� except�� �߻��ߴٸ�(SIGURG��� except) 
				strLen=recv(hRecvSock, buf, BUF_SIZE-1, MSG_OOB);//MSG_OOB�� recv�� ���. 
				buf[strLen]=0;
				printf("Urgent message: %s\n", buf);
			}
			
			if(FD_ISSET(hRecvSock, &readCopy)){//���� �Ϲ����� �޽��� �����̶�� 
				strLen=recv(hRecvSock, buf, BUF_SIZE-1, 0);//revd
				if(strLen==0){//EOF�� �ݱ� 
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
