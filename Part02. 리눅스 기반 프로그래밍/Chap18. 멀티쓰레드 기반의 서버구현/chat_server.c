#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100
#define MAX_CLNT 256

void * handle_clnt(void * arg);
void send_msg(char * msg, int len);
void error_handling(char * msg);

int clnt_cnt=0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;//뮤텍스 

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutx, NULL);//뮤텍스 default 옵션으로 생성 
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//TCP_IP
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
		
	while(1){
		clnt_adr_sz=sizeof(clnt_adr);
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		
		pthread_mutex_lock(&mutx);//SYN critical section 
		clnt_socks[clnt_cnt++]=clnt_sock;//클라이언트 소켓을 clnt_socks에 추가. 
		pthread_mutex_unlock(&mutx);//FIN critical section
		
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);//쓰레드를 생성 후 
		pthread_detach(t_id);//detach!
		printf("Connected client IP: %s\n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void * handle_clnt(void * arg){//clnt_sock의 핸들이 인자로 전송. detach됨. clnt_sock으로부터 읽을게 없을때까지 모든 client를 중개한다. (모든 client별로 실행) 
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	//아 서버 프로그램이니까 client_sock에서 읽은 내용을 모든 client에게 전파하는 역활을 handle_clnt에서 맡는듯. 완벽한 이해는 클라이언트 보면 될듯. 
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)//클라이언트소켓으로부터 읽을 메시지가 있다면 msg에 담아 
		send_msg(msg, str_len);//send_msg함수를 호출. (저장된 모든 클라이언트에게 메시지 전송) 
		
	pthread_mutex_lock(&mutx);//critical section! 
	for(i=0; i<clnt_cnt; i++){//send_msg로 작업이 끝난 후, 현재의 클라이언트를 clnt_sock에서 삭제하기 위한 코드..라는데..흠.. 뭔가 최적화 가능할 것 같은 느낌.. 뭐 index얻기위해 i를 이용하는건..뭐..좋지. 
		if(clnt_sock==clnt_socks[i]){
			while(i++<clnt_socks[i])//덮어쓰기 
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;//clnt_socks에서 현재 client제거 후 clnt_cnt도 -1 
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;//작업 종료 
}
void send_msg(char * msg, int len){
	int i;
	pthread_mutex_lock(&mutx);//critical section을 설정하여 
	for(i=0; i<clnt_cnt; i++)//모든 클라이언트에게 msg를 전송. 
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
