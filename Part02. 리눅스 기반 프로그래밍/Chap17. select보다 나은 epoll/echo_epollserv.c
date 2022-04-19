#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 100
#define EPOLL_SIZE 50
void error_handling(char *buf);

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];
	
	struct epoll_event *ep_events;//�������� Epoll�� �̿��ϱ� ���ؼ� ��� �� �̺�Ʈ �߻��� ���Ǵ� epoll_event�� ����. 
	struct epoll_event event;//
	int epfd, event_cnt;//epoll�� ���� ��ũ���͸� ������ ���� epfd 
	
	if(argc!=2){
		printf("usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
		
	epfd=epoll_create(EPOLL_SIZE);//EPOLL_SIZE�� epoll������ �����.(�ü������) 
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//EPOLL_SIZE��ŭ�� ep_events�� ���� �Ҵ��Ѵ�. �̺�Ʈ�߻��� ���⿡ ���� ���� 
	
	event.events=EPOLLIN;//�ַ� ����� �̺�Ʈ�� EPOLLIN��, �����Ͱ� ���ŵ� ���¸� �����ϰ� 
	event.data.fd=serv_sock;//���������� ���ϵ�ũ���͸� data.fd�� �����Ѵ�. 
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);//epfd�� serv_sock���ϵ�ũ���͸� �߰��ϸ�, event(���� EPOLLIN)�߻��� malloced events�� ���´�. 
	
	while(1){
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);//epfd���� �̺�Ʈ�� �߻��ϱ� ��ٸ���, �߻��� malloced ep_events�� ���´�. 
		if(event_cnt==-1){//����ó�� 
			puts("epoll_wait() error");
			break;
		}
		
		for(i=0; i<event_cnt; i++){//�߻��� �̺�Ʈ ��ŭ �ݺ��� �ϴµ� 
			if(ep_events[i].data.fd==serv_sock){//ep_events�� ���������̶�� 
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);//Ŭ���̾�Ʈ�κ����� ��û�� ���� 
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);//Ŭ���̾�Ʈ�� epfd�� ADD 
				printf("connected client: %d\n", clnt_sock);
			} else{//Ŭ���̾�Ʈ �����̶�� 
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);//�����͸� Ŭ���̾�Ʈ�κ��� ���� �ڿ� 
				if(str_len==0){//EOF�ÿ� 
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);//ó���� �Ϸ�Ǿ��⿡ epfd���� Ŭ���̾�Ʈ�� ����� 
					close(ep_events[i].data.fd);//�ݴ´�. 
					printf("closed client: %d\n", ep_events[i].data.fd);
				} else{//���� �����Ͱ� �ִٸ� 
					write(ep_events[i].data.fd, buf, str_len);//Ŭ���̾�Ʈ�κ��� ���� buffer�� ������ �ٽ� Ŭ���̾�Ʈ(ep_evnets[i])�� ������. (echo!)
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}//SELECT�� �޸� ��� ��ũ���͸� for�� ���Ⱦ �Ǹ�, �ü���ܿ��� �������ֱ⿡ wait�ÿ����� ����� ������ �Ź� ������ �ʿ䰡 ����. 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
