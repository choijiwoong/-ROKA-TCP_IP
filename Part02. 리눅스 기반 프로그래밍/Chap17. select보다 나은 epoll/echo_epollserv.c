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
	
	struct epoll_event *ep_events;//리눅스의 Epoll을 이용하기 위해서 등록 및 이벤트 발생시 사용되는 epoll_event를 선언. 
	struct epoll_event event;//
	int epfd, event_cnt;//epoll의 파일 디스크립터를 저장할 변수 epfd 
	
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
		
	epfd=epoll_create(EPOLL_SIZE);//EPOLL_SIZE로 epoll공간을 만든다.(운영체제에서) 
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//EPOLL_SIZE만큼의 ep_events를 동적 할당한다. 이벤트발생시 여기에 묶일 예정 
	
	event.events=EPOLLIN;//주로 사용할 이벤트로 EPOLLIN즉, 데이터가 수신된 상태를 지정하고 
	event.data.fd=serv_sock;//서버소켓의 파일디스크립터를 data.fd에 지정한다. 
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);//epfd에 serv_sock파일디스크립터를 추가하며, event(현재 EPOLLIN)발생시 malloced events에 묶는다. 
	
	while(1){
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);//epfd에서 이벤트가 발생하기 기다리며, 발생시 malloced ep_events에 묶는다. 
		if(event_cnt==-1){//예외처리 
			puts("epoll_wait() error");
			break;
		}
		
		for(i=0; i<event_cnt; i++){//발생한 이벤트 만큼 반복을 하는데 
			if(ep_events[i].data.fd==serv_sock){//ep_events가 서버소켓이라면 
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);//클라이언트로부터의 요청을 수락 
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);//클라이언트를 epfd에 ADD 
				printf("connected client: %d\n", clnt_sock);
			} else{//클라이언트 소켓이라면 
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);//데이터를 클라이언트로부터 읽은 뒤에 
				if(str_len==0){//EOF시에 
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);//처리가 완료되었기에 epfd에서 클라이언트를 지우고 
					close(ep_events[i].data.fd);//닫는다. 
					printf("closed client: %d\n", ep_events[i].data.fd);
				} else{//읽힌 데이터가 있다면 
					write(ep_events[i].data.fd, buf, str_len);//클라이언트로부터 읽은 buffer의 내용을 다시 클라이언트(ep_evnets[i])로 보낸다. (echo!)
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}//SELECT와 달리 모든 디스크립터를 for로 안훑어도 되며, 운영체제단에서 관리해주기에 wait시에관찰 대상의 정보를 매번 전달할 필요가 없다. 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
