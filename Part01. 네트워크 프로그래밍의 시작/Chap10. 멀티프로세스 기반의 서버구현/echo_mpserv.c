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

int main(int argc, char *argv[]){//(Chap04의 에코 클라이언트와 실행하능) 
	int serv_sock, clnt_sock;//에코 서버소켓과 클라이언트 소켓 
	struct sockaddr_in serv_adr, clnt_adr;//둘의 주소를 저장할 변수 
	
	pid_t pid;//프로세스 아이디 
	struct sigaction act;//sigaction함수 호출을 위한 구조체 
	socklen_t adr_sz;//주소 길이 
	int str_len, state;//문장 길이, 상태 
	char buf[BUF_SIZE];
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	act.sa_handler=read_childproc;//act의 callback설정 
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;//기본설정 
	state=sigaction(SIGCHLD, &act, 0);//자식 프로세스의 종료 시 SIGCHLD발생하게 sigaction통하여 등록 
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//서버 소켓 TCP로 생성 
	
	memset(&serv_adr, 0, sizeof(serv_adr)); //서버 주소 초기화 
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);//기본 
	serv_adr.sin_port=htons(atoi(argv[1]));//전달 포트 
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)//서버 소켓과 주소 연결 
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)//연결가능상태 
		error_handling("listen() error");
		
	while(1){
		adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);//클라이언트의 요청을 accept 
		if(clnt_sock==-1)
			continue;
		else
			puts("new client connected...");
		
		pid=fork();//fork! 키포인트는 fork이전에 accept를 호출하며 만들어진 소켓 파일 디스크립터가 자식 프로세승 복사된다는 것이다. 
		if(pid==-1){//fork실패 예외처리 
			close(clnt_sock);
			continue;
		}
		
		if(pid==0){//자식프로세스라면  
			close(serv_sock);//서버소켓을 닫고 클라이언트와 연결된 소켓만을 사용한다. 
			while((str_len=read(clnt_sock, buf, BUF_SIZE))!=0)//클라이언트로부터 수신한 메시지를 
				write(clnt_sock, buf, str_len);//전송한다(echo) 
				
			close(clnt_sock);//클라이언트 소켓을 닫는다. 단순한 분리의 목적 뿐 아닌, 하나의 소켓에 두개의 파일 디스크립터가 존재하면 둘 다 종료되어야 소켓이 소멸가능하기에 분리하는 것이다 
			puts("client disconnected...");
			return 0;
		}//만약 자식 프로세스가 종료될 경우 main process에서 act의 sa_handler로 지정된 read_childproc를 호출한다. 
		else
			close(clnt_sock);//클라이언트 소켓을 닫고 메인 프로세스에서는 서버소켓만을 관리한다. 
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
