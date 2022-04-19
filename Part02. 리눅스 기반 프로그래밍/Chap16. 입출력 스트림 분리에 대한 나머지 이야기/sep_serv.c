#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	FILE * readfp;
	FILE * writefp;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	char buf[BUF_SIZE]={0,};
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	listen(serv_sock, 5);
	clnt_adr_sz=sizeof(clnt_adr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
	readfp=fdopen(clnt_sock, "r");//FILE* 모드 즉, 입출력 스트림의 분리! 
	writefp=fdopen(clnt_sock, "w");
	
	fputs("FROM SERVER: Hi~ client?\n", writefp);
	fputs("I love all of the world\n", writefp);
	fputs("You are awesome!\n", writefp);
	fflush(writefp);
	
	fclose(writefp);//'w'모드 FILE*만 close하여 half-close(shutdown)의 기능을 유도해본다. 
	fgets(buf, sizeof(buf), readfp);//테스트를 위해 get과 (클라이언트 thank you메시지의 수신을 위함) 
	fputs(buf, stdout);//put을 수행 
	fclose(readfp);//나머지 닫기. 이론상 W만 닫혔기에 R에서 get하여 stdout으로 printf하는 부분은 문제가없어야 한다. 
	
	return 0;
}//결과를 확인해보니 Client의 thank you(FIN)이 도착하지 않았고, 단순히 W를 close하므써 half-close상태가 아닌 fully-close상태가 되었음을 확인할 수 있다! 
//FILE* 안에서 Half-close를 사용하기 위한 방법을 다음소주제에서 알아보자. 
