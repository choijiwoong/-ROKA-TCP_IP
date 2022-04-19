#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]){
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;
	FILE * readfp;
	FILE * writefp;
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
		puts("Connected............");
		
	readfp=fdopen(sock, "r");//fdopen(FILE*로의 변환)함수로 서버와 연결된 소켓을 변환. 
	writefp=fdopen(sock, "w");//각각의 모드를 가진 FILE*로 변환. 
	while(1){//수신 과정에서 표준 입출력함수를 이용하기에 \0을 따로 입력할 필요가 없다는 추가적인 장점. 
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);//fgets를 이용해서 stdin에서부터 message로 입력. 
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
			
		fputs(message, writefp);//message입력을 표준입출력함수 fputs를 이용하여 writefp(FILE* "w"로 변환된 소켓 파일디스크립터)에 출력 
		fflush(writefp);//모드 전환과 즉각 전송보장을 위한 fflush호출(이것이 단점이 될 수도 있음) 
		fgets(message, BUF_SIZE, readfp);//"r"모드로 열린 FILE*을 message에서 readfp으로 표준 출력. 
		printf("Message from server: %s", message);
	}
	fclose(writefp);
	fclose(readfp);
	
	return 0;
} 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
