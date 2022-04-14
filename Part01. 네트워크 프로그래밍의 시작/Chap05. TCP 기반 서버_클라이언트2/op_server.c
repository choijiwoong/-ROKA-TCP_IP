#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
#define OPSZ 4
void error_handling(char *message);
int calculate(int opnum, int opnds[], char oprator);

int main(int argc, char *argv[]){
	int serv_sock, clnt_sock;
	char opinfo[BUF_SIZE];//계산 및 통신에 사용할 주 변수 
	int result, opnd_cnt, i;//결과값, 피연산자 개수, for 
	int recv_cnt, recv_len;// 
	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)//서버 소켓 및 주소 바인딩 
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)//연결요청 대기 큐 활성화 
		error_handling("listen() error");
	clnt_adr_sz=sizeof(clnt_adr);
	
	for(i=0; i<5; i++){//5개의 클라이언트 요청을 수행하기 위함.(Question. 그럼 이게 연결요청 대기 큐 5를 말하는건 아닐거같은데.. ) 
		opnd_cnt=0;
		clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);//클라이언트 소켓과의 연결 
		read(clnt_sock, &opnd_cnt, 1);//피 연산자 개수(1바이트)만 읽기 
		
		recv_len=0;//received length 수신한 데이터 길이 
		while((opnd_cnt*OPSZ+1)>recv_len){//opnd_cnt(피연산자개수)*4바이트정수+1(미만나타내기위함) 
			recv_cnt=read(clnt_sock, &opinfo[recv_len], BUF_SIZE-1);//나머지 다 읽어버리기. read는 성공시 읽은 길이 반환 
			recv_len+=recv_cnt;//읽은 길이 반영 
		}
		result=calculate(opnd_cnt, (int*)opinfo, opinfo[recv_len-1]);//피연산자 개수, 피연산자 담긴거, 수신길이-1에 연산자 종류 담김. 
		write(clnt_sock, (char*)&result, sizeof(result));//클라이언트 소켓에 결과값 전송 
		close(clnt_sock);//클라이언트 소켓 종료 
	}
	close(serv_sock);
	return 0;
}

int calculate(int opnum, int opnds[], char op){//피연산자 개수, 피연산자들, 연산자종류 
	int result=opnds[0], i;//첫번째 수를 result에 저장 
	switch(op){//연산자에 따라 
		case '+':
			for(i=1; i<opnum; i++)//피연산자 개수만큼 연산 실행 
				result+=opnds[i];
			break;
		
		case '-':
			for(i=1; i<opnum; i++)
				result-=opnds[i];
			break;
		
		case '*':
			for(i=1; i<opnum; i++)
				result*=opnds[i];
			break;
	}
	return result;//결과값 리턴 
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
