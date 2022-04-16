#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0
void error_handling(char *message);

int main(int argc, char *argv[]){//단순 echo서버 
	int serv_sock, clnt_sock;
	char message[30];
	int option, str_len;
	socklen_t optlen, clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");
		
	/*
	만약 four-hand shaking과정을 통해 정상적인 FIN을 수신하고 최종적인 ACK메시지를 보낸 뒤 바로 서버를 꺼버렸다고 가정했을때, 상대가 ACK를 수신받지 못해 다시 FIN을 보내면 영원히 B는 ACK를 못받게된다. 
	time-wait는 행여나 ACK메시지를 상대가 못받아서 다시 FIN메시지를 보낼 경우를 대비하여 기다리는 시간이다. 다만 이는 네트워크의 상황이 원활하지 못하다면
	FIN을 받을때마다 Time-wait를 재가동시키기에 언제까지 지속될지 장담할 수 없다. 이는 재빨리 재가동해야하는 서버의 경우 치명적이기에
	 그러한 경우 SO_REUSEADDR옵션을 True로 활성화 시켜 현재 time-wait중인 포트의 재사용(재가동)을 가능케 한다. 
	*/
	oplen=sizeof(option);
	option=TRUE;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);//SOL_SOCKET계츨 SO_REUSEADDR(포트재사용)옵션을 TRUE로 변경 
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen error");
	clnt_adr_sz=sizeof(clnt_adr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
	while((str_len=read(clnt_sock, message, sizeof(message)))!=0){
		write(clnt_sock, message, str_len);
		write(1, message, str_len);
	}
	close(clnt_sock);
	close(serv_sock);
	return 0;
} 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
