#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char* argv[]){
	int sock;//요청할 소켓 하나 선언 
	struct sockaddr_in serv_addr;//서버소켓 주소 (찾아갈 예정) 
	char message[30];//메시지 저장소 
	int str_len;
	
	if(argc!=3){
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	} 
	
	sock=socket(PF_INET, SOCK_STREAM, 0);//소켓 생성(소켓 생성 뒤에 bind&listen이 이어지면 서버소켓, connect가 이어지면 클라이언트 소켓이 된다) 
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&serv_addr, 0, sizeof(serv_addr));//서버주소 0으로 초기화 
	serv_addr.sin_family=AF_INET;//속성 설정 
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)//클라이언트 소켓과 서버프로그램 연결을 요청. 
		error_handling("connect() error!");
		
	str_len=read(sock, message, sizeof(message)-1);//서버소켓에서 메시지를 읽어 길이 반환 
	if(str_len==-1)
		error_handling("read() error!");
	
	printf("Message from server: %s\n", message);//메시지 출력 
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
