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
pthread_mutex_t mutx;//���ؽ� 

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_sz;
	pthread_t t_id;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	pthread_mutex_init(&mutx, NULL);//���ؽ� default �ɼ����� ���� 
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
		clnt_socks[clnt_cnt++]=clnt_sock;//Ŭ���̾�Ʈ ������ clnt_socks�� �߰�. 
		pthread_mutex_unlock(&mutx);//FIN critical section
		
		pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);//�����带 ���� �� 
		pthread_detach(t_id);//detach!
		printf("Connected client IP: %s\n", inet_ntoa(clnt_adr.sin_addr));
	}
	close(serv_sock);
	return 0;
}

void * handle_clnt(void * arg){//clnt_sock�� �ڵ��� ���ڷ� ����. detach��. clnt_sock���κ��� ������ ���������� ��� client�� �߰��Ѵ�. (��� client���� ����) 
	int clnt_sock=*((int*)arg);
	int str_len=0, i;
	char msg[BUF_SIZE];
	//�� ���� ���α׷��̴ϱ� client_sock���� ���� ������ ��� client���� �����ϴ� ��Ȱ�� handle_clnt���� �ôµ�. �Ϻ��� ���ش� Ŭ���̾�Ʈ ���� �ɵ�. 
	while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)//Ŭ���̾�Ʈ�������κ��� ���� �޽����� �ִٸ� msg�� ��� 
		send_msg(msg, str_len);//send_msg�Լ��� ȣ��. (����� ��� Ŭ���̾�Ʈ���� �޽��� ����) 
		
	pthread_mutex_lock(&mutx);//critical section! 
	for(i=0; i<clnt_cnt; i++){//send_msg�� �۾��� ���� ��, ������ Ŭ���̾�Ʈ�� clnt_sock���� �����ϱ� ���� �ڵ�..��µ�..��.. ���� ����ȭ ������ �� ���� ����.. �� index������� i�� �̿��ϴ°�..��..����. 
		if(clnt_sock==clnt_socks[i]){
			while(i++<clnt_socks[i])//����� 
				clnt_socks[i]=clnt_socks[i+1];
			break;
		}
	}
	clnt_cnt--;//clnt_socks���� ���� client���� �� clnt_cnt�� -1 
	pthread_mutex_unlock(&mutx);
	close(clnt_sock);
	return NULL;//�۾� ���� 
}
void send_msg(char * msg, int len){
	int i;
	pthread_mutex_lock(&mutx);//critical section�� �����Ͽ� 
	for(i=0; i<clnt_cnt; i++)//��� Ŭ���̾�Ʈ���� msg�� ����. 
		write(clnt_socks[i], msg, len);
	pthread_mutex_unlock(&mutx);
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
