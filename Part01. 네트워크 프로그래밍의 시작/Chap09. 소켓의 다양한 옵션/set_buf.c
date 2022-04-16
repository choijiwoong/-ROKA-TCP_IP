#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
	int sock;
	int snd_buf=1024*3, rcv_buf=1024*3;
	int state;
	socklen_t len;
	
	sock=socket(PF_INET, SOCK_STREAM, 0);//기본 생성시 당연히 여러 옵션 세팅되며, 그중하나가 SO_RCVBUF와SO_SNDBUF크기! 
	state=setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));//setsockopt사용하여 SO_RCVBUF값을 rcv_buf값으로 세팅 
	if(state)
		error_handling("setsockopt() error!");
		
	state=setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));//sendbuffer크기 rcv_buf로 세팅 
	if(state)
		error_handling("setsockopt() error!");
		
	len=sizeof(snd_buf);
	state=getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);//그다음 get SO_SNDBUF값 
	if(state)
		error_handling("getsockopt() error!");
		
	len=sizeof(rcv_buf);
	state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
	if(state)
		error_handling("getsockopt() error!");
	
	printf("Input buffer size: %d\n", rcv_buf);//변경된 입출력버퍼 길이 세팅값 출력 
	printf("Output buffer size: %d\n", snd_buf);//하지만 1024*3=3072와는 달리, 6144로 세팅되어있다.
	//입출력 버퍼는 상당히 중요한 영역이기에 우리가 전달하는 opt value를 무조건 설정하는 것이 아닌, 제안하는 것이다. 고로 정확한 값은 아니지만 어느정도 반영된 값을 확인할 수 있다. 
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
