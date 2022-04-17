#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100
void error_handling(char *buf);

int main(int argc, char *argv[]) {
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;
	struct timeval timeout;//select에서 timeout값 전달을 위한 구조체 
	fd_set reads, cpy_reads;//파일디스크립터 컨테이너
	 
	socklen_t adr_sz;
	int fd_max, str_len, fd_num, i;
	char buf[BUF_SIZE];
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
		error_handling("listen() error");//소켓만들고 주소 초기화, 바인딩 가용상태 
		
	FD_ZERO(&reads);//fd_set초기화 
	FD_SET(serv_sock, &reads);//서버소켓에 할당된 디스크립터를 세팅 
	fd_max=serv_sock;//최대 파일 디스크립터(추후 +1하여 사용예정) 
	
	while(1){
		cpy_reads=reads;//fd_set복사 
		timeout.tv_sec=5;//timeout설정을위한 값 
		timeout.tv_usec=5000;
		//2번째 인자로 서버의 디스크립터가 포함된 것의 데이터 수신여부를 검사하는데, 이의 수신은 연결요청으로 해석한다(서버소켓 데이터 수신이니) 
		if((fd_num=select(fd_max+1, &cpy_reads, 0, 0, &timeout))==-1)//select를 수행하는데, 블로킹없이 전송가능한지를 관심으로 추가. 
			break;//오류처리 
		if(fd_num==0)//timeout시 다시 
			continue;
		//-1, 0을 제외하곤 이벤트가 생겨서 관심등록된 cpy_reads에 변화가 생긴 것. 
		for(i=0; i<fd_max+1; i++){//전체 파일 디스크립터에 대해 아래의 작업을 수행 
			if(FD_ISSET(i, &cpy_reads)){//변화가 있나요? 
				if(i==serv_sock){//서버소켓에 변화가 있다면 
					adr_sz=sizeof(clnt_adr);
					clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);//클라이언트 수락 
					FD_SET(clnt_sock, &reads);//fd_set에 클라이언트 소켓의 파일 디스크립터 세팅1 
					
					if(fd_max<clnt_sock)//최대길이 갱싱 
						fd_max=clnt_sock;
					printf("connected client: %d\n", clnt_sock);
				} else{//서버 외의 변화가 있다면(클라이언트의 변화가 있다면, 즉 수신할 메시지가 있다면) 
					str_len=read(i, buf, BUF_SIZE);//버퍼를 읽는다 
					if(str_len==0){//전송된 것이 0(Null, EOF)이라면 (close request)
						FD_CLR(i, &reads);//해당 디스크립터를 0으로 초기화하고 
						close(i);//해당 디스크립터를 닫는다. 
						printf("closed client: %d\n", i);
					} else{//내용이 전달되었다면 buf에 작성. 
						write(i, buf, str_len);
					}
				}
			}
		}
	}
	close(serv_sock);
	return 0;
}

void error_handling(char *buf){
	fputs(buf, stderr);
	fputc('\n', stderr);
	exit(1);
}
