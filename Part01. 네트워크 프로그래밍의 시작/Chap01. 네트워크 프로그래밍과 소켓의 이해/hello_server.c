#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);//오류핸들링 

int main(int argc, char *argv[]){
	int serv_sock;//소켓 선언 
	int clnt_sock;
	
	struct sockaddr_in serv_addr;//sockaddr_in타입의 주소 선언 
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;//클라이언트 주소의 길이를 따로 담을 변수 선언 
	
	char message[]="Hello World!"; 
	
	if (argc!=2){//?대충 호출 잘못. 인자로 포트가 들어가는듯. 고로 호출 시 잘못호출하면 이 포트 사용하라고 알려주는듯 
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//서버소켓 정의 
	if(serv_sock==-1)//오류시 핸들링 
		error_handling("socket() error");
	
	memset(&serv_addr, 0, sizeof(serv_addr));//서버 주소 0으로 초기화 
	serv_addr.sin_family=AF_INET;//서버주소 속성설정 
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1)//서버소켓을 서버주소에 개설
		error_handling("bind() error");
		
	if(listen(serv_sock, 5)==-1)//가용상태로 전환(이를 서버 소켓 혹은 리스닝 소켓이라 부른다) 
		error_handling("listen() error");
	
	clnt_addr_size=sizeof(clnt_addr);//클라이언트 주소길이 정의 
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);//클라이언트 요청을 수락 
	if(clnt_sock==-1)
		error_handling("accept() error");
	
	write(clnt_sock, message, sizeof(message));//클라이언트 소켓에 메시지 write 
	close(clnt_sock);
	close(serv_sock);
	
	return 0;//최종적으로 연결 요청을 기다리는 서버의 상태가 된다. 
}

void error_handling(char *message){//stderr사용 오류메시지 전달&종료 
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
