#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
	int serv_sock;
	int clnt_sock;
	
	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;
	
	char message[]="Hello World!";
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);//마중물, 문지기 느낌의 소켓이기에 실질 연결시 clnt전용으로 소켓 추가 생성. 
	if(serv_sock==-1)
		error_handling("socket() error");
		
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;//network family전달 
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);//address로 컴퓨터 기존 주소를 htonl을 사용하여 long타입 host->network 즉 Big Endian으로 변환 하여 주소로 설정. s_addr사용은 0채우는 조건 충족 위함 
	serv_addr.sin_port=htons(atoi(argv[1]));//string타입 숫자를 정수로, short타입값 Big Endian으로
	
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)//바인드를 통해 주소(포트 등..)와 소켓을 연결 
		error_handling("bind() error");
	
	if(listen(serv_sock, 5)==-1)//연결가능상태 활성화 with 연결 요청 대기 큐 사이즈 5
		error_handling("listen() error") ;
		
	clnt_addr_size=sizeof(clnt_addr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);//클라이언트 요청을 serv_sock과 연결하며 해당 클라이언트 전용 소켓 추가생성하여 연결 
	if(clnt_sock==-1)
		error_handling("accept() error");
		
	write(clnt_sock, message, sizeof(message));
	close(clnt_sock);
	close(serv_sock);
	
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
