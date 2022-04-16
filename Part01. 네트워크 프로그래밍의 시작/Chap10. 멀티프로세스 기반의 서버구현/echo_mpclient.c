#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);//Ŭ���̾�Ʈ������ ����� ��ƾ �и� for �о�Ȱ��(��ٸ��� �ʰ� ������ �� �� �ְԲ�) 
void write_routine(int sock, char *buf);

int main(int argc, char *argv[]){
	int sock;
	pid_t pid;//���μ��� ���̵� ���� ���� 
	char buf[BUF_SIZE];
	struct sockaddr_in serv_adr;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)//������ Ŭ���̾�Ʈ ������ ���� 
		error_handling("connect() error");
		
	pid=fork();//fork
	if(pid==0)//�ڽ��� ���� ��ƾ ���(������ ���Ǹ� ����) 
		write_routine(sock, buf);
	else//�θ�� �б� ��ƾ ��� 
		read_routine(sock, buf);
		
	close(sock);
	return 0;
} 

void read_routine(int sock, char *buf){//�θ� ���μ������� ���� 
	while(1){
		int str_len=read(sock, buf, BUF_SIZE);//������ �����͸� ���� 
		if(str_len==0)
			return 0;
			
		buf[str_len]=0;
		printf("Message from server: %s", buf);//�����κ��� ���� �޽��� ��� 
	}
}

void write_routine(int sock, char *buf){//�ڽ� ���μ������� ���� 
	while(1){
		fgets(buf, BUF_SIZE, stdin);
		if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){//EOD���� �� fork�� �����̹Ƿ� �ѹ��� close�� EOF������ �ȵ� �� �ֱ⿡ EOF�� ������ shutdown�� ���� ��������� ��Ÿ����. 
			shutdown(sock, DHUT_WR);//�ݱ⸦ ���Ѵٸ� �����带 half-close(����޽����� ����ڰ� �����⿡ ����� ���޵Ǿ�� �Ѵ�) 
			return;
		}
		write(sock, buf, strlen(buf));//stdin���� �Է¹��� ������ ������ ���� 
	}//������ close�ϴ� ������ �ʿ����� ������. �ϳ��� ���Ͽ��� �о��� �� ��. �ٸ� FIN�� ���Ͽ� write�ÿ� half-close�� ����Ѵ�. 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
