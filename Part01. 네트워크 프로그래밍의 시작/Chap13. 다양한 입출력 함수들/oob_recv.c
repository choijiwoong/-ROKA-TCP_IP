#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 30
void error_handling(char *message);
void urg_handler(int signo);

int acpt_sock;
int recv_sock;

int main(int argc, char *argv[]){
	struct sockaddr_in recv_adr, serv_adr;
	int str_len, state;
	socklen_t serv_adr_sz;
	struct sigaction act;//스그널 발생을 처리하기 위한 sigcation구조체 
	char buf[BUF_SIZE];
	if(argc!=2){
		printf("Usage: %s <port> \n", argv[0]);
		exit(1);
	}
	
	act.sa_handler=urg_handler;//시그널 발생시 호출할 콜백함수 
	sigemptyset(&act.sa_mask);//나머지 0으로 초기화 in sigaction's member 
	act.sa_flags=0;
	
	acpt_sock=socket(PF_INET, SOCK_STREAM, 0);//서버소켓생성 
	memset(&recv_adr, 0, sizeof(recv_adr));
	recv_adr.sin_family=AF_INET;
	recv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	recv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(acpt_sock, (struct sockaddr*)&recv_adr, sizeof(recv_adr))==-1)//연결 
		error_handling("bind() error");
	listen(acpt_sock, 5);//가용 
	
	serv_adr_sz=sizeof(serv_adr);
	recv_sock=accept(acpt_sock, (struct sockaddr*)&serv_adr, &serv_adr_sz);//클라이언트의 요청을 수락 
	
	fcntl(recv_sock, F_SETOWN, getpid());//[이는 파일 디스크립터 컨트롤에 사용되는 함수로] recv_socket에서 발생하는 ISGURG시그널 처리 프로세스를 getpid가 반환하는 ID의 프로세스로 변경하겠다는 뜻이다. 
	state=sigaction(SIGURG, &act, 0);//URG시그널 발생 시 act호출. 그 결과값 state에 저장(긴급메시지 전송시 SIGURG를 발생시킨다) 
	//[하나의 소켓에 대한 파일 디스크립터가 여러 프로세스에서 소유할 수 있으므로, 정확히 시그널 처리시 어느 프로세스에서 처리할지를 지정해주어야 한다.]
	while((str_len=recv(recv_sock, buf, sizeof(buf), 0))!=0){//receive socket으로부터 메시지를 수신한다. 
		if(str_len==-1)
			continue;
		buf[str_len]=0;
		puts(buf);//출력[TCP는 별도의 통신 경로를 제공하지 않기에 TCP의 Urgent mode라는 것을 이용하여 전송할 뿐이다. 딱 1바이트만 반환한다.] 
	}
	close(recv_sock);
	close(acpt_sock);
	return 0;
}

void urg_handler(int signo){//긴급메시지 송신 시그널 발생시 호출 
	int str_len;
	char buf[BUF_SIZE];
	str_len=recv(recv_sock, buf, sizeof(buf)-1, MSG_OOB);//메시지를 MSG_OOB옵션으로 받는다. 
	buf[str_len]=0;
	printf("Urgent message: %s\n", buf);//긴급메시지 출력 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
