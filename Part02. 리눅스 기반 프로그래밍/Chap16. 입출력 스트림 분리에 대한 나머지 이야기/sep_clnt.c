#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
	int sock;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_addr;
	
	FILE * readfp;
	FILE * writefp;
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(atoi(argv[2]));
	
	connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	readfp=fdopen(sock, "r");//FILE*은 모드 분리해야하다보니 자연스럽게 입출력 스트림의 분리 발생! 
	writefp=fdopen(sock, "w");
	
	while(1){//서버에서 fputs로 보낸 메시지 3개를 받음. 서버는 그 후 'W'를 close. 
		if(fgets(buf, sizeof(buf), readfp)==NULL)
			break;
		fputs(buf, stdout);
		fflush(stdout);//즉각전송보장 
	}
	
	fputs("FROM CLIENT: Thank you!\n", writefp);//서버 w닫은 후 클라이언트에서 서버로 발송. 
	fflush(writefp);
	fclose(writefp); fclose(readfp);
	return 0;
} 
