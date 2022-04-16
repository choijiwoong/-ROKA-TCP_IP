#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
	int tcp_sock, udp_sock;
	int sock_type;
	socklen_t optlen;
	int state;
	
	optlen=sizeof(sock_type);
	tcp_sock=socket(PF_INET, SOCK_STREAM, 0);
	udp_sock=socket(PF_INET, SOCK_DGRAM, 0);
	printf("SOCK_STREAM: %d\n", SOCK_STREAM);
	printf("SOCK_DGRAM: %d\n", SOCK_DGRAM);
	
	state=getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);//tcp_sock의 option을 get할 건데, SOL_SOCKET계층에 SO_TYPE옵션의 값을 sock_type에 저장하라. 
	if(state)
		error_handling("getsockopt() error!");
	printf("Socket type one: %d\n", sock_type);
	
	state=getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, (void*)&sock_type, &optlen);//udp_sock의 SOL_SOCKET계층 SO_TYPE값을 sock_type에 저장하라 
	if(state)
		error_handling("getsocket() error!");
	printf("Socket type two: %d\n", sock_type);//SO_TYPE은 확인만 가능하고 변경이 불가능한 옵션 중 하나다. SO_TYPE 즉 소켓의 타입은 소켓 생성 이후 변경이 불가능하다는 것을 의미한다. 
	
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
