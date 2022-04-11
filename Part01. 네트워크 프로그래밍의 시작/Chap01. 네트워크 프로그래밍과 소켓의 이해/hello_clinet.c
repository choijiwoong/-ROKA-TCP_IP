#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char* argv[]){
	int sock;//��û�� ���� �ϳ� ���� 
	struct sockaddr_in serv_addr;//�������� �ּ� (ã�ư� ����) 
	char message[30];//�޽��� ����� 
	int str_len;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	} 
	
	sock=socket(PF_INET, SOCK_STREAM, 0);//���� ����(���� ���� �ڿ� bind&listen�� �̾����� ��������, connect�� �̾����� Ŭ���̾�Ʈ ������ �ȴ�) 
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&serv_addr, 0, sizeof(serv_addr));//�����ּ� 0���� �ʱ�ȭ 
	serv_addr.sin_family=AF_INET;//�Ӽ� ���� 
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)//Ŭ���̾�Ʈ ���ϰ� �������α׷� ������ ��û. 
		error_handling("connect() error!");
		
	str_len=read(sock, message, sizeof(message)-1);//�������Ͽ��� �޽����� �о� ���� ��ȯ 
	if(str_len==-1)
		error_handling("read() error!");
	
	printf("Message from server: %s\n", message);//�޽��� ��� 
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
