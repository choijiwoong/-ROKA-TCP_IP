#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);//클라이언트에서의 입출력 루틴 분리 for 분업활동(기다리지 않고 전송을 할 수 있게끔) 
void write_routine(int sock, char *buf);

int main(int argc, char *argv[]){
	int sock;
	pid_t pid;//프로세스 아이디를 담을 변수 
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
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)//서버와 클라이언트 소켓을 연결 
		error_handling("connect() error");
		
	pid=fork();//fork
	if(pid==0)//자식은 쓰기 루틴 담당(구현의 편의를 위함) 
		write_routine(sock, buf);
	else//부모는 읽기 루틴 담당 
		read_routine(sock, buf);
		
	close(sock);
	return 0;
} 

void read_routine(int sock, char *buf){//부모 프로세스에서 실행 
	while(1){
		int str_len=read(sock, buf, BUF_SIZE);//소켓의 데이터를 저장 
		if(str_len==0)
			return 0;
			
		buf[str_len]=0;
		printf("Message from server: %s", buf);//서버로부터 얻은 메시지 출력 
	}
}

void write_routine(int sock, char *buf){//자식 프로세스에서 실행 
	while(1){
		fgets(buf, BUF_SIZE, stdin);
		if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){//EOD전달 시 fork된 상태이므로 한번의 close로 EOF전달이 안될 수 있기에 EOF의 전달을 shutdown을 통해 명시적으로 나타낸다. 
			shutdown(sock, DHUT_WR);//닫기를 원한다면 쓰기모드를 half-close(종료메시지를 사용자가 보내기에 제대로 전달되어야 한다) 
			return;
		}
		write(sock, buf, strlen(buf));//stdin에서 입력받은 내용을 서버에 전송 
	}//별도로 close하는 과정은 필요하지 않은듯. 하나의 소켓에서 분업을 할 뿐. 다만 FIN을 위하여 write시에 half-close를 사용한다. 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
