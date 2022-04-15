#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[]){//uecho_server.c와 함께 사용 
	int sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t adr_sz;
	
	struct sockaddr_in serv_adr, from_adr;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);//UDP socket
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));//connect on UDP socket
	
	while(1){
		fputs("Insert message(q to quit): ", stdout);
		fgets(message, sizeof(message), stdin);
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
			
		write(sock, message, strlen(message));//일반적인 write함수를 사용. 현재UDP소켓은 이미 주소에 연결(connect)되어 있기에 write로 소켓에 쓴다 
		str_len=read(sock, message, sizeof(message)-1);//마찬가지로 read를 쓴다. 이는 UDP를 하나의 호스트와 오랜 시간 데이터 송수신시 적합하다. 
		
		message[str_len]=0;
		printf("Message from server: %s", message);
	}
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
