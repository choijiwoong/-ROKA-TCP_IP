#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 30
void error_handling(char *message);
//Sender�� �ּҿ� ��Ʈ�� ��Ƽĳ��Ʈ �������� �ٲٷ�, IPPROTO_IP������ IP_MULTICAST_TTL�ɼǿ� TTL���� �־��ָ� �غ� ��ģ��. 
int main(int argc, char *argv[]){//��Ƽĳ��Ʈ Sender�� �Ϲ� UDP���� ���α׷��� ū ���̰� ����. 
	int send_sock;
	struct sockaddr_in mul_adr;
	int time_live=TTL;//TTL�� �󸶳� ��Ŷ�� �ָ� ������ ���ΰ��� ��Ÿ����, ����͸� �ѹ� ��ĥ������ 1�� �����Ѵ�. 
	FILE *fp;
	char buf[BUF_SIZE];
	if(argc!=3){
		printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);//�׷� �����ǿ� ��Ƽĳ��Ʈ ��Ʈ�� �޴´�. 
		exit(1);
	}
	
	send_sock=socket(PF_INET, SOCK_DGRAM, 0);
	memset(&mul_adr, 0, sizeof(mul_adr));
	mul_adr.sin_family=AF_INET;
	mul_adr.sin_addr.s_addr=inet_addr(argv[1]);//��Ƽĳ��Ʈ ������ ���(������ �ּҰ� �ƴϴ�!) 
	mul_adr.sin_port=htons(atoi(argv[2]));//��Ƽĳ��Ʈ ��Ʈ ��� 
	
	setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&time_live, sizeof(time_live));//�������� ������ IPPROTO_IP, IP_MULTICAST_TTL�ɼ��� TTL���� �����Ѵ�. 
	if((fp=fopen("news.txt", "r"))==NULL)
		error_handling("fopen() error");
		
	while(!feof(fp)){//Broadcasting (feof�� ������ EOF������ Ȯ���Ѵ� �� ������ NULL���� check )
		fgets(buf, BUF_SIZE, fp);
		sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*)&mul_adr, sizeof(mul_adr));//�ּҷ� ���۸� ����. by UDP 
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
