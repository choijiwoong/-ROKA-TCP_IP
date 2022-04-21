#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);

char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];

int main(int argc, char * argv[]){
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t snd_thread, rcv_thread;
	void * thread_return;//thread의 리턴을 받을 void형 포인터 
	if(argc!=4){
		printf("Usage: %s <IP> <port> <name> \n", argv[0]);
		exit(1);
	}
	
	sprintf(name, "[%s]", argv[3]);//name에 main인자로 받은 clnt_name을 넣는다. 
	sock=socket(PF_INET, SOCK_STREAM, 0);//TCP-IP
	
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
		error_handling("connect() error");
		
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);//메시지 전송부와 
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);//메시지 수신부 스레드 실행 
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	close(sock);
	
	return 0;
}

void * send_msg(void * arg){
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	while(1){
		fgets(msg, BUF_SIZE, stdin);//stdin으로부터 msg를 입력받고 
		if(!strcmp(msg, "q\n")||!strcmp(msg, "Q\n")){//종료메시지일 시 소켓을 종료하고 작업을 마무리한다. 
			close(sock);
			exit(0);
		}
		sprintf(name_msg, "%s %s", name, msg);//msg와 clnt 이름정보를 더불어 name_msg에 넣고  
		write(sock, name_msg, strlen(name_msg));//server socket에 전송한다. 
	}
	return NULL;
}

void * recv_msg(void * arg){
	int sock=*((int*)arg);
	char name_msg[NAME_SIZE+BUF_SIZE];
	int str_len;
	while(1){
		str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);//서버 소켓으로부터 수신받은 메시지를 name_msg에 저장하고, 
		if(str_len==-1)
			return (void*)-1;
		name_msg[str_len]=0;//null처리를 한 후 stdout에 표시한다. 
		fputs(name_msg, stdout);
	}
	return NULL;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
