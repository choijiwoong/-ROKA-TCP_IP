#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inet.h>
#include <sys.socket.h>

#define BUF_SIZE 24
void error_handling(char *message);

int main(int argc, char *argv[]){//Chap04의 echo_sever.c에 표준입출력사용 
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	int str_len, i;
	
	struct sockaddr_in serv_adr;
	struct sockaddr_in clnt_adr;
	socklen_t clnt_adr_sz;
	FILE* readfp;//표준 입출력함수 사용을 위한 포인터 마련 
	FILE* writefp;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addrs.s_addr=htonl(INADDRY_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error");
	clnt_adr_sz=sizeof(clnt_adr);
	
	for(i=0; i<5; i++){//5개의 클라이언트에 대하여 순차적으로 연결(통신)을 진행 
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);//클라이언트 소켓의 생성(해당 소켓의 파일 디스크립터 지정) 
		if(clnt_sock==-1)
			error_handling("accept() error");
		else
			printf("Connected client %d\n", i+1);
			
		readfp=fdopen(clnt_sock, "r");//클라이언트 소켓의 파일 디스크립터를 fdopen을 이용하여 FILE*로 변환하는데 모드는 읽기. 
		writefp=fdopen(clnt_sock, "w");//마찬가지로 모드는 쓰기.(readfp와 writefp는 모두 위에 정의해둔 FILE*형식이다) 
		while(!feof(readfp)){//제대로 변환이 되었고 데이터가 EOF가 아니라면(이상이 없다면) 
			fgets(message, BUF_SIZE, readfp);//fgets표준 입출력 함수를 FILE*에 수행. readfp에서 BUF_SIZE만큼 가져와서 message에 저장. 
			fputs(message, writefp);//message(버퍼)의 값을 writefp에 출력. 마찬가지로 표준입출력함수 fputs를 사용(추가적인 버퍼 이용) 
			fflush(writefp);//fflush를 호출하여 당장 클라이언트로 전송되게끔 보장한다.(표준입출력 함수 버퍼에서 fflush) 
		}
		fclose(readfp);//파일 포인터형식이기에 fclose로 닫기. 
		fclose(writefp);
	}
	close(serv_sock);//서버소켓 종료 
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
