#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int fds[2];
	
	pid_t pid;
	struct sigaction act;
	socklen_t adr_sz;
	int str_len, state;
	char buf[BUF_SIZE];
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	act.sa_handler=read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	state=sigaction(SIGCHLD, &act, 0);
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
		
	pipe(fds);//fds에 파이프 생성 
	pid=fork();//프로세스 분리(복제) 
	if(pid==0){//자식 프로세스는 echomsg.txt에 파이프로 읽는 것들을 read하여 fwrite 
		FILE *fp=fopen("echomsg.txt", "wt");
		char msgbuf[BUF_SIZE];
		int i, len;
		
		for(i=0; i<10; i++){
			len=read(fds[0], msgbuf, BUF_SIZE);
			fwrite((void*)msgbuf, 1, len, fp);
		}
		fclose(fp);
		return 0;//파일을 다 쓴 뒤 종료 
	}
	
	while(1){//부모 프로세스의 경우 
		adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);//클라이언트 소켓과 연결하여 
		if(clnt_sock==-1)
			continue;
		else
			puts("new client connected...");
			
		pid=fork();//분리후 
		if(pid==0){//자식은 서버소켓닫고 
			close(serv_sock);
			while((str_len=read(clnt_sock, buf, BUF_SIZE))!=0){//클라이언트로부터 읽고, 파이프에 쓰기 
				write(clnt_sock, buf, str_len);
				write(fds[1], buf str_len);
			}
			close(clnt_sock);
			puts("client disconnected...");
			return 0;
		}
		else//부모는 클라이언트 소켓 닫고 
			close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}
void read_childproc(int sig){//자식프로세스가 반환 혹은 종료됐을 경우 호출될 함수 
	pid_t pid;
	int status;
	pid=waitpid(-1, &status, WNOHANG);//자식 프로세스의 종료를 임의로 확인한다. 
	printf("removed proc id: %d\n", pid);//종료된 프로세스 pid출력 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
