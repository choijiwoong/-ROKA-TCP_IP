#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
vodi error_handling(char *message);

int main(int argc, char *argv[]){//��Ƽĳ��Ʈ�� MBone(Multicast Backbone)�̶�� ���� ��Ʈ��ũ�� ������� �����Ѵ�.(����̱⿡ send�� �����ϰ� �ʰ� receiver�� �����ϸ� �������� ���� ���� �ִ�) 
	int recv_sock;
	int str_len;
	char buf[BUF_SIZE];
	struct sockaddr_in adr;
	struct ip_mreq join_adr;//Broadcasting�ÿ� IP_ADD_MEMBERSHIP�� �ɼ� ������ ���� ���Ǵ� ����ü�� in_addrŸ������ ��Ƽĳ��Ʈ �׷��� �ּ������� ������ ȣ��Ʈ�� �ּ������� ����ִ�. 
	if(argc!=3){
		printf("Usage: %s <GroupIP> <PORT>\n", argv[0]);//�׷� �ּҿ� ��Ʈ�� ����. 
		exit(1);
	}
	
	recv_sock=socket(PF_INET, SOCK_DGRAM, 0);//UDP��� 
	memset(&adr, 0, sizeof(adr));
	adr.sin_family=AF_INET;
	adr.sin_addr.s_addr=htonl(INADDR_ANY);//���� ��ǻ���� �ּ� 
	adr.sin_port=htons(atoi(argv[2]));//���ڷ� ���� ��Ʈ(�翬�� sender�� ��Ʈ�� ���ƾ��Ѵ�!) 
	
	if(bind(recv_sock, (struct sockaddr*)&adr, sizeof(adr))==-1)
		error_handling("bind() error");
		
	join_adr.imr_multiaddr.s_addr=inet_addr(argv[1]);//IP_ADD_MEMBERSHIP�� �ɼ��� �����ϱ����� �׷��� �ּҰ�, �׷쿡 ������ ȣ��Ʈ�� �ּҸ� �����Ѵ�. 
	join_adr.imr_interface.s_addr=htonl(INADDR_ANY);
	
	setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*)&join_adr, sizeof(join_adr));//���� ������ join_adr�� ������ IPPROTO_IP������ IP_ADD_MEMBERSHIP�� ���� �����Ѵ�. 
	//��Ƽĳ��Ʈ�׷쿡 �����ϸ�, �׷����� ���۵Ǵ� �����͸� ������ �غ� ��ģ��.
	
	while(1){
		str_len=recvfrom(recv_sock, buf, BUF_SIZE-1, 0, NULL, 0);//���������� ���� �޴µ�...? �ƿ� recv_sock�� �ɼ����� ��� �� ���ļ� �����ϴ°ǰ�..? �׷���. ȣ��Ʈ�� �ּ������� �� �ʿ䰡 ���ٸ� NULL�� 0�� ����. 
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
