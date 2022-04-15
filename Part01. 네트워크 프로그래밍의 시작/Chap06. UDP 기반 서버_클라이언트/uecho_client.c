#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char * argv[]){
	int sock;
	char message[BUF_SIZE];
	int str_len;
	socklen_t adr_sz;//우리 소켓으로 수신 시 길이지정에 사용할 변수 
	
	struct sockaddr_in serv_adr, from_adr;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_DGRAM, 0);
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));//서버 주소 초기화. 서버 소켓은 필요X 
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	while(1){
		fputs("Insert message(q to quit): ", stdout);
		fgets(message, sizeof(message), stdin);
		if(!strcmp(message, "q\n") || strcmp(message, "Q\n"))
			break;
			
		sendto(sock, message, strlen(message), 0, (struct sockaddr*)&serv_adr, sizeof(serv_adr));//UDP sock이용, 메시지를 전달 to 서버주소.(서버주소는 recv from으로 client주소 확인가능 thanks to sock) 
		adr_sz=sizeof(from_adr);
		str_len=recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);//server에서 발신자 정보를 채워줄 구조체 주소 전달(어디서 온 데이터인지 서버가 전송) 
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
//좀 햇갈려서 정리하면, sendto(socket, socket_address)는 말 그대로 socket send to socket_addredd이고 이를 받을 때 recv_from으로 받게되는데
//recv(sock, from_adr)말 그대로 sock이 수신한 message는 from from_adr에서 온 것이라는 의미이다. sendto시엔 서버 주소를 넣어 보내면
//recv시에 from_adr구조체에 sentto시 전송된 서버주소정보가 저장된다. 즉 메시지가 어디서 온 것인지 기록되어있다. 
