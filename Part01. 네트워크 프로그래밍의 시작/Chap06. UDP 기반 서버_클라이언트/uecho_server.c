#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[]){
	int serv_sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t clnt_adr_sz;
	
	struct sockaddr_in serv_adr, clnt_adr;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_DGRAM, 0);//Define UDP Socket
	if(serv_sock==-1)
		error_handling("UDP socket creation error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));//adr init
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)//bind UDP socket with adr
		error_handling("bind() error");
	
	while(1){//무한 루프 
		clnt_adr_sz=sizeof(clnt_adr);//모든 데이터 수신. 아래 구조체로 발신자 정보(client address)를 받아 해당 주소로 sendto할 예정이다. 
		str_len=recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);//without listen&accept, receive
		sendto(serv_sock, message, str_len, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);//and send by client address
	}//serv_sock에서 clnt_adr로 message를 전송. client에선 recvfrom으로 자기꺼로 오는거에 대한 발신자 정보(serv_sock)을 알 수 있다. 
	close(serv_sock);
	return 0;
} 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
