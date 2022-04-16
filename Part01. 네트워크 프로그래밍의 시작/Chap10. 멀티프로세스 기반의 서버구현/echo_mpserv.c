#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[]){//(Chap04�� ���� Ŭ���̾�Ʈ�� �����ϴ�) 
	int serv_sock, clnt_sock;//���� �������ϰ� Ŭ���̾�Ʈ ���� 
	struct sockaddr_in serv_adr, clnt_adr;//���� �ּҸ� ������ ���� 
	
	pid_t pid;//���μ��� ���̵� 
	struct sigaction act;//sigaction�Լ� ȣ���� ���� ����ü 
	socklen_t adr_sz;//�ּ� ���� 
	int str_len, state;//���� ����, ���� 
	char buf[BUF_SIZE];
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	act.sa_handler=read_childproc;//act�� callback���� 
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;//�⺻���� 
	state=sigaction(SIGCHLD, &act, 0);//�ڽ� ���μ����� ���� �� SIGCHLD�߻��ϰ� sigaction���Ͽ� ��� 
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//���� ���� TCP�� ���� 
	
	memset(&serv_adr, 0, sizeof(serv_adr)); //���� �ּ� �ʱ�ȭ 
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);//�⺻ 
	serv_adr.sin_port=htons(atoi(argv[1]));//���� ��Ʈ 
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)//���� ���ϰ� �ּ� ���� 
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)//���ᰡ�ɻ��� 
		error_handling("listen() error");
		
	while(1){
		adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);//Ŭ���̾�Ʈ�� ��û�� accept 
		if(clnt_sock==-1)
			continue;
		else
			puts("new client connected...");
		
		pid=fork();//fork! Ű����Ʈ�� fork������ accept�� ȣ���ϸ� ������� ���� ���� ��ũ���Ͱ� �ڽ� ���μ��� ����ȴٴ� ���̴�. 
		if(pid==-1){//fork���� ����ó�� 
			close(clnt_sock);
			continue;
		}
		
		if(pid==0){//�ڽ����μ������  
			close(serv_sock);//���������� �ݰ� Ŭ���̾�Ʈ�� ����� ���ϸ��� ����Ѵ�. 
			while((str_len=read(clnt_sock, buf, BUF_SIZE))!=0)//Ŭ���̾�Ʈ�κ��� ������ �޽����� 
				write(clnt_sock, buf, str_len);//�����Ѵ�(echo) 
				
			close(clnt_sock);//Ŭ���̾�Ʈ ������ �ݴ´�. �ܼ��� �и��� ���� �� �ƴ�, �ϳ��� ���Ͽ� �ΰ��� ���� ��ũ���Ͱ� �����ϸ� �� �� ����Ǿ�� ������ �Ҹ갡���ϱ⿡ �и��ϴ� ���̴� 
			puts("client disconnected...");
			return 0;
		}//���� �ڽ� ���μ����� ����� ��� main process���� act�� sa_handler�� ������ read_childproc�� ȣ���Ѵ�. 
		else
			close(clnt_sock);//Ŭ���̾�Ʈ ������ �ݰ� ���� ���μ��������� �������ϸ��� �����Ѵ�. 
	}
	close(serv_sock);
	return 0;
}

void read_childproc(int sig){//�ڽ����μ����� ��ȯ Ȥ�� ������� ��� ȣ��� �Լ� 
	pid_t pid;
	int status;
	pid=waitpid(-1, &status, WNOHANG);//�ڽ� ���μ����� ���Ḧ ���Ƿ� Ȯ���Ѵ�. 
	printf("removed proc id: %d\n", pid);//����� ���μ��� pid��� 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
