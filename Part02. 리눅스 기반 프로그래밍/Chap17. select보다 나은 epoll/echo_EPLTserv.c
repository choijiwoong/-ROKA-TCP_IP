#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#define BUF_SIZE 4
#define EPOLL_SIZE 50
void error_handling(char *buf);

int main(int argc, char *argv[]){//레벨 트리거 
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t adr_sz;
	int str_len, i;
	char buf[BUF_SIZE];
	
	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;
	
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
		
	epfd=epoll_create(EPOLL_SIZE);
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//동적할당 
	
	event.events=EPOLLIN;//이벤트 데이터수신 
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);//서버소켓 epfd에 등록(파일 디스크립터 저장소 in 운영체제) 
	
	while(1){
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);//데이터 수신시 ep_events에 엮을예정 
		if(event_cnt==-1){
			puts("epoll_wait() error");
			break;
		}
		
		puts("return epoll_wait");//**EPOLL_WAIT의 호출횟수를 확인하기 위함(레벨 트리거라면 데이터 조금씩 수신할때마다 호출될거임(이벤트발생으로 간주하기에). 또한 이 과정을 잘 보기위해 버퍼크기 4로 축소) ***
		for(i=0; i<event_cnt; i++){//이벤트 발생(모든)에 대하여 
			if(ep_events[i].data.fd==serv_sock){//서버소켓이면 
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);//뭐 등록 그대로 
				printf("connected client: %d\n", clnt_sock);
			} else{//클라이언트면 
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);//클라이언트 소켓으로부터 데이터 buf에 읽어서 
				if(str_len==0){
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);//처리후 epoll저장소에서 삭제 
					close(ep_events[i].data.fd);
					printf("closed client: %d\n", ep_events[i].data.fd);
				} else{
					write(ep_events[i].data.fd, buf, str_len);//데이터 buf를 되돌려 client에 보낸다(echo) 
				}
			}
		}
	}
	close(serv_sock);
	close(epfd);
	return 0;
}//결과는 예상과 같이 이벤트 등록이 여러번 이뤄지기에 다수 호출된다(epoll_wait). event.events=EPOLLIN|EPOLLET으로의 변경으로 엣지트리거로 변경이 가능하지만, 추가작업이 필요하다. 
//(참고로 SELECT는 레벨트리거로 작동한다고 한다.) p.381질문에 대한 답: 버퍼가 너무 작아서? 
void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
