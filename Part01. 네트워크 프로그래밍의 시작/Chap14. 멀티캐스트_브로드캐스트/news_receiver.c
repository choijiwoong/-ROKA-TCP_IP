#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
vodi error_handling(char *message);

int main(int argc, char *argv[]){//멀티캐스트는 MBone(Multicast Backbone)이라는 가상 네트워크를 기반으로 동작한다.(방송이기에 send를 실행하고 늦게 receiver를 실행하면 수신하지 못할 수도 있다) 
	int recv_sock;
	int str_len;
	char buf[BUF_SIZE];
	struct sockaddr_in adr;
	struct ip_mreq join_adr;//Broadcasting시에 IP_ADD_MEMBERSHIP의 옵션 변경을 위해 사용되는 구조체로 in_addr타입으로 멀티캐스트 그룹의 주소정보와 가입할 호스트의 주소정보를 담고있다. 
	if(argc!=3){
		printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);//그룹 주소와 포트를 전달. 
		exit(1);
	}
	
	recv_sock=socket(PF_INET, SOCK_DGRAM, 0);//UDP기반 
	memset(&adr, 0, sizeof(adr));
	adr.sin_family=AF_INET;
	adr.sin_addr.s_addr=htonl(INADDR_ANY);//현재 컴퓨터의 주소 
	adr.sin_port=htons(atoi(argv[2]));//인자로 받은 포트(당연히 sender의 포트와 같아야한다!) 
	
	if(bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr))==-1)
		error_handling("bind() error");
		
	join_adr.imr_multiaddr.s_addr=inet_addr(argv[1]);//IP_ADD_MEMBERSHIP의 옵션을 변경하기위해 그룹의 주소값, 그룹에 가입할 호스트의 주소를 저장한다. 
	join_adr.imr_interface.s_addr=htonl(INADDR_ANY);
	
	setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));//위에 세팅한 join_adr의 값으로 IPPROTO_IP계층의 IP_ADD_MEMBERSHIP의 값을 변경한다. 
	//멀티캐스트그룹에 가입하며, 그룹으로 전송되는 데이터를 수신할 준비를 마친다.
	
	while(1){
		str_len=recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);//서버소켓은 값을 받는데...? 아예 recv_sock의 옵션으로 모든 걸 마쳐서 생략하는건가..? 그러네. 호스트와 주소정보를 알 필요가 없다면 NULL과 0을 전달. 
		if(str_len<0)
			break;
		buf[str_len]=0;
		fputs(buf, stdout);
	}
	close(recv_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
