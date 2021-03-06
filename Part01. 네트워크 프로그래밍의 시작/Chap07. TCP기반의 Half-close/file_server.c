#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[]) {//오류 핸들링 상당부분 생략 
	int serv_sd, clnt_sd;
	FILE* fp;
	char buf[BUF_SIZE];
	int read_cnt;
	
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	fp=fopen("file_server.c", "rb");
	serv_sd=socket(PF_INET, SOCK_STREAM, 0);
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=PF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr));
	listen(serv_sd, 5);
	
	clnt_adr_sz=sizeof(clnt_adr);
	clnt_sd=accept(serv_sd, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);//클라이언트 전용 소켓 
	
	while(1){
		read_cnt=fread((void*)buf, 1, BUF_SIZE, fp);//file_server.c를 BUF_SIZE만큼 읽어서 
		if(read_cnt<BUF_SIZE){//버퍼가 모자르게 찼다면(파일의 끝에 달했다면( 
			write(clnt_sd, buf, read_cnt);//읽은 크기만큼 쓰고 escape 
			break;
		}
		write(clnt_sd, buf, BUF_SIZE);//BUF_SIZE만큼 write 
	}
	
	shutdown(clnt_sd, SHUT_WR);//half-close write stream. Client에서 fwrite를 마친 후 Thank you 메시지를 보낼 것이기에 write stream만 닫는다. 
	read(clnt_sd, buf, BUF_SIZE);//읽기 가능. thank you 메시지 클라이언트로부터 수신 
	printf("Message from client: %s\n", buf);
	
	fclose(fp);
	close(clnt_sd);
	close(serv_sd);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
