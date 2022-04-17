#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);
//Sender는 주소와 포트응 멀티캐스트 전용으로 바꾸로, IPPROTO_IP계층의 IP_MULTICAST_TTL옵션에 TTL값을 넣어주며 준비를 마친다. 
int main(int argc, char *argv[]){//멀티캐스트 Sender는 일반 UDP소켓 프로그램과 큰 차이가 없다. 
	int send_sock;
	struct sockaddr_in mul_adr;
	int time_live=TTL;//TTL은 얼마나 패킷을 멀리 전달할 것인가를 나타내며, 라우터를 한번 거칠때마다 1씩 감소한다. 
	FILE *fp;
	char buf[BUF_SIZE];
	if(argc!=3){
		printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);//그룹 아이피와 멀티캐스트 포트를 받는다. 
		exit(1);
	}
	
	send_sock=socket(PF_INET, SOCK_DGRAM, 0);
	memset(&mul_adr, 0, sizeof(mul_adr));
	mul_adr.sin_family=AF_INET;
	mul_adr.sin_addr.s_addr=inet_addr(argv[1]);//멀티캐스트 아이피 등록(서버의 주소가 아니다!) 
	mul_adr.sin_port=htons(atoi(argv[2]));//멀티캐스트 포트 등록 
	
	setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));//프로토콜 레벨은 IPPROTO_IP, IP_MULTICAST_TTL옵션을 TTL값을 설정한다. 
	if((fp=fopen("news.txt", "r"))==NULL)
		error_handling("fopen() error");
		
	while(!feof(fp)){//Broadcasting (feof는 파일의 EOF인지를 확인한다 즉 파일이 NULL인지 check )
		fgets(buf, BUF_SIZE, fp);
		sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_adr, sizeof(mul_adr));//주소로 버퍼를 보냄. by UDP 
		sleep(2);
	}
	close(fp);
	close(send_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
