#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4//4바이트 정수 
void error_handling(char* message);

int main(int argc, char *argv[]){
	int sock;
	char opmsg[BUF_SIZE];
	int result, opnd_cnt, i;
	struct sockaddr_in serv_adr;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected........");
		
	fputs("Operand count: ", stdout);//피연산자 개수 
	scanf("%d", &opnd_cnt);
	opmsg[0]=(char)opnd_cnt;//opmsg의 첫번째값으로 피연산자 개수를 받는다. 
	
	for(i=0; i<opnd_cnt; i++){
		printf("Operand %d: ", i+1);
		scanf("%d", (int*)&opmsg[i*OPSZ+1]);//4바이트 정수를 저장할 것이기에 %d로 받고 int로 캐스팅 후 저장 
	}
	fgetc(stdin);//fflush
	fputs("Operator: ", stdout);
	scanf("%c", &opmsg[opnd_cnt*OPSZ+1]);//operatior은 1개만 1바이트로 받을것이기에 전체 수 저장된거에 +1한 곳에 %c로 저장 
	write(sock, opmsg, opnd_cnt*OPSZ+2);//null문자까지 해서 +1하여 socket을 통해 서버에 전송한다. (여러번의 write도 가능) 
	read(sock, &result, RLT_SIZE);//RESULT_SIZE(즉, 결과는 4바이트 정수임)만큼 연산결과를 가져온다. (수신할 데이터의 크기를 알기에 한번의 read로도 가능) 
	
	printf("Operation result: %d\n", result);
	close(sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
