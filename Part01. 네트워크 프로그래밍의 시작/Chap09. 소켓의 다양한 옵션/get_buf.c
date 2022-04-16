#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
	int sock;
	int snd_buf, rcv_buf, state;
	socklen_t len;
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	len=sizeof(snd_buf);
	state=getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);//SOL_SOCKET계층에 SendBuffer관련 옵션을 참조 
	if(state)
		error_handling("getsockopt() error");
		
	len=sizeof(rcv_buf);
	state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
	if(state)
		error_handling("getsockpot() error");
		
	printf("Input buffer size: %d\n", rcv_buf);//애초에 int값이지..! 
	printf("Output buffer size: %d\n", snd_buf);//소켓 생성시 기본적으로 세팅되는 입출력 버퍼의 크기 확인 
	
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
