#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[]){//host1로 전송할 예정인데 3번 전송함. 
	int sock;
	char msg1[]="Hi!";
	char msg2[]="I'm another UDP host!";
	char msg3[]="Nice to meet you";
	
	struct sockaddr_in your_adr;
	socklen_t your_adr_sz;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);//UDP용 소켓 
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&your_adr, 0, sizeof(your_adr));//your_adr 주소 0초기화 후 인자로 온 주소와 포트 설정. 
	your_adr.sin_family=AF_INET;
	your_adr.sin_addr.s_addr=inet_addr(argv[1]);
	your_adr.sin_port=htons(atoi(argv[2]));
	
	sendto(sock, msg1, sizeof(msg1), 0, (struct sockaddr*)&your_adr, sizeof(your_adr));//인자로 들어와 초기화했던 주소로 UDP소켓 자동 세팅 버전으로 msg보내기. 
	sendto(sock, msg2, sizeof(msg2), 0, (struct sockaddr*)&your_adr, sizeof(your_adr));
	sendto(sock, msg2, sizeof(msg3), 0, (struct sockaddr*)&tour_adr, sizeof(your_adr));
	close(sock);
	
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
