#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[]){//host2에서 3번 전송한 데이터를 3회에 걸쳐 수신함. 
	int sock;
	char message[BUF_SIZE];
	struct sockaddr_in my_adr, your_adr;
	socklen_t adr_sz;
	int str_len, i;
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&my_adr, 0, sizeof(my_adr));
	my_adr.sin_family=AF_INET;
	my_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	my_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(sock, (struct sockaddr*)&my_adr, sizeof(my_adr))==-1)
		error_handling("bind() error");
		
	for(i=0; i<3; i++){
		sleep(5);//sleep을 통해 TCP와의 차이점을 알 수 있는데, sleep을 통해 실질적으로 host2가 3번의 데이터를 이미 보낸 상태가 된다. 
		adr_sz=sizeof(your_adr);//TCP의 경우 한번의 입력함수호출로 다 수신할 수 있지만, UDP는 3회의 recvfrom호출이 요구되며 짝을 맞춰야하는 이유이다. 
		str_len=recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&your_adr, &adr_sz);
		
		printf("Message %d: %s\n", i+1, message);
	}
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
