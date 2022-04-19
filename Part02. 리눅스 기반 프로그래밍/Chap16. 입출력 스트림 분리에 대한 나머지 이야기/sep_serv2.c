#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]){//FILE*에서도 Half-close를 위해선 dup&dup2로 파일디스크립터를 복사하여 연쇄삭제를 막으며, shutdown을 SHUT_WR옵션으로 호출시켜 직접적으놀 Half-close 즉 EOF를 발생시킨다. 
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
	
	readfp=fdopen(clnt_sock, "r");//소켓 디스크립터를 FILE*로 'r' 
	writefp=fdopen(dup(clnt_sock), "w");//마찬가지를 파일디스크립터 복사를 통해 'w'생성. 
	
	fputs("FROM SERVER: Hi~ client?\n", writefp);
	fputs("I love all of the world\n", writefp);
	fputs("You are awesome!\n", writefp);
	fflush(writefp);
	
	//shutdown을 하므로서 Half-close가 진행되거 클라이언트로 EOF가 전달된다.	
	shutdown(fileno(writefp), SHUT_WR);//<복사된 파일 디스크립터의 수에 상관없이 EOF의 전송을 동반하는 Half-close를 진행하기 위해서는 shutdown함수를 호출해야한다> 
	fclose(writefp);//쓰기모드의 FILE*을 fileno로서 파일디스크립터 변환 후, shutdown함수에 SHUT_WR옵션으로 Half-close를 직접호출 후 writefp를 닫는다. 
	
	fgets(buf, sizeof(buf), readfp);
	fputs(buf, stdout);
	fclose(readfp);
	
	return 0;
} 
