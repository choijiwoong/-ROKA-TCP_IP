#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50
void setnonblockingmode(int fd);
void error_handling(char *buf);

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];
	
	struct epoll_event *ep_events;//malloc�� ���� 
	struct epoll_event event;//�̺�Ʈ ������ ���� 
	int epfd, event_cnt;//epoll ���� ��ũ���� ����� 
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
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
		
	epfd=epoll_create(EPOLL_SIZE);//epoll����� ����(�ü��) 
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//�����Ҵ�. �̺�Ʈ �߻��� ���� ���� 
	
	setnonblockingmode(serv_sock);//serv_sock�� ���ϵ�ũ������ ������ fcntl�Լ��� �̿��Ͽ� O_NONBLOCK�ɼ��� �߰��Ѵ�. 
	event.events=EPOLLIN;//������ ������ �̺�Ʈ�� ��� 
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);//���������� epfd(epoll file descriptors)�� ����Ѵ�.(�׷����� serv_sock������ �ι��ֳ�.. ����°���ڴ� �����������, �׹�°�� ��������� �̺�Ʈ. �̺�Ʈ ������ ��� ���ӽ��� �����ϳ�����) 
	
	while(1){
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);//����ŷ(�Ͽ����� ��ٸ��� �� ���� �ֱ⿡..) 
		if(event_cnt==-1){
			puts("epoll_wait() error");
			break;
		}
		
		puts("return epoll_wait");
		for(i=0; i<event_cnt; i++){//��� �̺�Ʈ �߻��� ���Ͽ�(������ �׳� �н�) 
			if(ep_events[i].data.fd==serv_sock){//������ 
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				setnonblockingmode(clnt_sock);//Ŭ���̾�Ʈ�� �ͺ������ �ٲٷ� 
				event.events=EPOLLIN|EPOLLET;//�̺�Ʈ�� ����Ʈ�������� ������ְ� 
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);//�߰� 
				printf("connected client: %d\n", clnt_sock);
			} else{
				while(1){//Ŭ���̾�Ʈ���. �׸��� ����Ʈ���ſ� �޸� while�� ���� ������ �Է¹��ۿ� �����ϴ� �����͸� ��� �Ѳ����� �����ؾ��ϱ� ������ read�� �ݺ���Ų��. 
					str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);//���ۿ� �����ڿ� 
					if(str_len==0){//EOF��� 
						epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);//���� 
						close(ep_events[i].data.fd);
						printf("closed client: %d\n", ep_events[i].data.fd);
						break;
					} else if(str_len<0){//�׷��ٰ� read�� -1�� ��ȯ�ϰ�, errno�� EAGAIN���� ���õǾ��ٸ� 
						if(errno==EAGAIN)//�����͸� ��� �о� ���� ��쿡 �ش��ϱ⿡ break�� �������´�. 
							break;
					} else{
						write(ep_events[i].data.fd, buf, str_len);//echo
					}
				}//�����͸� �ѹ��� ó���ϱ⿡ epoll_wait�� 5ȸ�� �������� ���ۿ� �����Ͽ� 5�� ȣ��Ǿ���. 
			}
		}
	}
	close(serv_sock); close(epfd);
	return 0;
}

void setnonblockingmode(int fd){//non-blocking������ ��ȯ.  
	int flag=fcntl(fd, F_GETFL, 0);//���ڷ� ���� file descriptor�� Ư��(flag)�� ������ 
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);//������ ����(flag)���� O_NONBLOCK �� ����ŷ��带 �߰��Ͽ� ����. 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
