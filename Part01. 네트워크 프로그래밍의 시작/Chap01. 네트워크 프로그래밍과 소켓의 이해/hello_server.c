#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);//�����ڵ鸵 

int main(int argc, char *argv[]){
	int serv_sock;//���� ���� 
	int clnt_sock;
	
	struct sockaddr_in serv_addr;//sockaddr_inŸ���� �ּ� ���� 
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;//Ŭ���̾�Ʈ �ּ��� ���̸� ���� ���� ���� ���� 
	
	char message[]="Hello World!"; 
	
	if (argc!=2){//?���� ȣ�� �߸�. ���ڷ� ��Ʈ�� ���µ�. ��� ȣ�� �� �߸�ȣ���ϸ� �� ��Ʈ ����϶�� �˷��ִµ� 
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//�������� ���� 
	if(serv_sock==-1)//������ �ڵ鸵 
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));//���� �ּ� 0���� �ʱ�ȭ 
	serv_addr.sin_family=AF_INET;//�����ּ� �Ӽ����� 
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)//���������� �����ּҿ� ����
		error_handling("bind() error");
		
	if(listen(serv_sock, 5)==-1)//������·� ��ȯ(�̸� ���� ���� Ȥ�� ������ �����̶� �θ���) 
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);//Ŭ���̾�Ʈ �ּұ��� ���� 
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);//Ŭ���̾�Ʈ ��û�� ���� 
	if(clnt_sock==-1)
		error_handling("accept() error");
	
	write(clnt_sock, message, sizeof(message));//Ŭ���̾�Ʈ ���Ͽ� �޽��� write 
	close(clnt_sock);
	close(serv_sock);
	
	return 0;//���������� ���� ��û�� ��ٸ��� ������ ���°� �ȴ�. 
}

void error_handling(char *message){//stderr��� �����޽��� ����&���� 
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
