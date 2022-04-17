#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
//다이렉트 브로드캐스팅의 경우 255.255.255.255가 아니라 현재 IP주소에서 끝자리 빼고 전송하면 된다. 
int main(int argc, char *argv[]){//255.255.255.255 9190을 입력할 예정. 그럼 현재 포트에서 마지막 4번째자리 모두에게 전송. 이게 local 브로드캐스팅. 
	int send_sock;
	struct sockaddr_in broad_adr;
	FILE *fp;
	char buf[BUF_SIZE];
	int so_brd=1;
	if(argc!=3){
		printf("Usage: %s <Broadcast IP> <PORT>\n", argv[0]);
		exit(1);
	}
	
	send_sock=socket(PF_INET, SOCK_DGRAM, 0);
	memset(&broad_adr, 0, sizeof(broad_adr));
	broad_adr.sin_family=AF_INET;
	broad_adr.sin_addr.s_addr=inet_addr(argv[1]);
	broad_adr.sin_port=htons(atoi(argv[2]));
	
	setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void*)so_brd, sizeof(so_brd));//소켓의 SOL_SOCKET의 SO_BROADCAST옵션을 1로 설정한다. 
	if((fp=fopen("news.txt", "r"))==NULL)
		error_handling("fopen() error");
		
	while(!feof(fp)){//feof는 파일의 EOF인지를 확인한다 즉 파일이 NULL인지 check 
		fgets(buf, BUF_SIZE, fp);
		sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&broad_adr, sizeof(broad_adr));
		sleep(2);
	}
	close(send_sock);
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
