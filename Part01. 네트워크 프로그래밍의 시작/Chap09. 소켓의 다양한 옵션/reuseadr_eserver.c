#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TRUE 1
#define FALSE 0
void error_handling(char *message);

int main(int argc, char *argv[]){//�ܼ� echo���� 
	int serv_sock, clnt_sock;
	char message[30];
	int option, str_len;
	socklen_t optlen, clnt_adr_sz;
	struct sockaddr_in serv_adr, clnt_adr;
	if(argc!=2){
		printf("Usage: %s <port>\n", argv[0]);
		exit(1);
	}
	
	serv_sock=socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error");
		
	/*
	���� four-hand shaking������ ���� �������� FIN�� �����ϰ� �������� ACK�޽����� ���� �� �ٷ� ������ �����ȴٰ� ����������, ��밡 ACK�� ���Ź��� ���� �ٽ� FIN�� ������ ������ B�� ACK�� ���ްԵȴ�. 
	time-wait�� �࿩�� ACK�޽����� ��밡 ���޾Ƽ� �ٽ� FIN�޽����� ���� ��츦 ����Ͽ� ��ٸ��� �ð��̴�. �ٸ� �̴� ��Ʈ��ũ�� ��Ȳ�� ��Ȱ���� ���ϴٸ�
	FIN�� ���������� Time-wait�� �簡����Ű�⿡ �������� ���ӵ��� ����� �� ����. �̴� �绡�� �簡���ؾ��ϴ� ������ ��� ġ�����̱⿡
	 �׷��� ��� SO_REUSEADDR�ɼ��� True�� Ȱ��ȭ ���� ���� time-wait���� ��Ʈ�� ����(�簡��)�� ������ �Ѵ�. 
	*/
	oplen=sizeof(option);
	option=TRUE;
	setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);//SOL_SOCKET���� SO_REUSEADDR(��Ʈ����)�ɼ��� TRUE�� ���� 
	
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_adr.sin_port=htons(atoi(argv[1]));
	
	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)))
		error_handling("bind() error");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen error");
	clnt_adr_sz=sizeof(clnt_adr);
	clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	
	while((str_len=read(clnt_sock, message, sizeof(message)))!=0){
		write(clnt_sock, message, str_len);
		write(1, message, str_len);
	}
	close(clnt_sock);
	close(serv_sock);
	return 0;
} 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
