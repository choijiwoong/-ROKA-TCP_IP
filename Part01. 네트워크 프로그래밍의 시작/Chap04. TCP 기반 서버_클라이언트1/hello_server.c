#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
	int serv_sock;
	int clnt_sock;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	char message[]="Hello World!";
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//���߹�, ������ ������ �����̱⿡ ���� ����� clnt�������� ���� �߰� ����. 
	if(serv_sock==-1)
		error_handling("socket() error");
		
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;//network family���� 
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//address�� ��ǻ�� ���� �ּҸ� htonl�� ����Ͽ� longŸ�� host->network �� Big Endian���� ��ȯ �Ͽ� �ּҷ� ����. s_addr����� 0ä��� ���� ���� ���� 
	serv_addr.sin_port=htons(atoi(argv[1]));//stringŸ�� ���ڸ� ������, shortŸ�԰� Big Endian����
	
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)//���ε带 ���� �ּ�(��Ʈ ��..)�� ������ ���� 
		error_handling("bind() error");
	
	if(listen(serv_sock, 5)==-1)//���ᰡ�ɻ��� Ȱ��ȭ with ���� ��û ��� ť ������ 5
		error_handling("listen() error") ;
		
	clnt_addr_size=sizeof(clnt_addr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);//Ŭ���̾�Ʈ ��û�� serv_sock�� �����ϸ� �ش� Ŭ���̾�Ʈ ���� ���� �߰������Ͽ� ���� 
	if(clnt_sock==-1)
		error_handling("accept() error");
		
	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
