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
	
	struct epoll_event *ep_events;//malloc을 위함 
	struct epoll_event event;//이벤트 지정을 위함 
	int epfd, event_cnt;//epoll 파일 디스크립터 저장소 
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
		
	epfd=epoll_create(EPOLL_SIZE);//epoll저장소 생성(운영체제) 
	ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//동적할당. 이벤트 발생시 묶일 예정 
	
	setnonblockingmode(serv_sock);//serv_sock의 파일디스크립터의 설정을 fcntl함수를 이용하여 O_NONBLOCK옵션을 추가한다. 
	event.events=EPOLLIN;//데이터 수신을 이벤트로 등록 
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);//서버소켓을 epfd(epoll file descriptors)에 등록한다.(그러고보니 serv_sock정보를 두번주네.. 세번째인자는 관찰대상지정, 네번째는 관찰대상의 이벤트. 이벤트 지정을 대상에 종속시켜 지정하나보넴) 
	
	while(1){
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);//논블라킹(하염없이 기다리게 될 수도 있기에..) 
		if(event_cnt==-1){
			puts("epoll_wait() error");
			break;
		}
		
		puts("return epoll_wait");
		for(i=0; i<event_cnt; i++){//모든 이벤트 발생에 대하여(없으면 그냥 패스) 
			if(ep_events[i].data.fd==serv_sock){//서버면 
				adr_sz=sizeof(clnt_adr);
				clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				setnonblockingmode(clnt_sock);//클라이언트도 넌블락으로 바꾸로 
				event.events=EPOLLIN|EPOLLET;//이벤트에 엣지트리거임을 명시해주고 
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);//추가 
				printf("connected client: %d\n", clnt_sock);
			} else{
				while(1){//클라이언트라면. 그리고 레벨트리거와 달리 while을 넣은 이유는 입력버퍼에 존재하는 데이터를 모두 한꺼번에 수신해야하기 때문에 read를 반복시킨다. 
					str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);//버퍼에 읽은뒤에 
					if(str_len==0){//EOF라면 
						epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);//삭제 
						close(ep_events[i].data.fd);
						printf("closed client: %d\n", ep_events[i].data.fd);
						break;
					} else if(str_len<0){//그러다가 read가 -1을 반환하고, errno가 EAGAIN으로 세팅되었다면 
						if(errno==EAGAIN)//데이터를 모두 읽어 들인 경우에 해당하기에 break로 빠져나온다. 
							break;
					} else{
						write(ep_events[i].data.fd, buf, str_len);//echo
					}
				}//데이터를 한번에 처리하기에 epoll_wait는 5회의 데이터의 전송에 대응하여 5번 호출되었다. 
			}
		}
	}
	close(serv_sock); close(epfd);
	return 0;
}

void setnonblockingmode(int fd){//non-blocking모드로의 변환.  
	int flag=fcntl(fd, F_GETFL, 0);//인자로 받은 file descriptor의 특성(flag)을 가져옴 
	fcntl(fd, F_SETFL, flag|O_NONBLOCK);//기존의 설정(flag)에서 O_NONBLOCK 즉 논블라킹모드를 추가하여 세팅. 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
