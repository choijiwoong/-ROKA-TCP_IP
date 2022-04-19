#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[]){
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;
	FILE * readfp;
	FILE * writefp;
	if(argc!=3){
		printf("Usage: %s <IP> <port>\n", argv[0]);
		exit(1);
	}
	
	sock=socket(PF_INET, SOCK_STREAM, 0);
	if(sock==-1)
		error_handling("socket() error");
		
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");
	else
		puts("Connected............");
		
	readfp=fdopen(sock, "r");//fdopen(FILE*���� ��ȯ)�Լ��� ������ ����� ������ ��ȯ. 
	writefp=fdopen(sock, "w");//������ ��带 ���� FILE*�� ��ȯ. 
	while(1){//���� �������� ǥ�� ������Լ��� �̿��ϱ⿡ \0�� ���� �Է��� �ʿ䰡 ���ٴ� �߰����� ����. 
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);//fgets�� �̿��ؼ� stdin�������� message�� �Է�. 
		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;
			
		fputs(message, writefp);//message�Է��� ǥ��������Լ� fputs�� �̿��Ͽ� writefp(FILE* "w"�� ��ȯ�� ���� ���ϵ�ũ����)�� ��� 
		fflush(writefp);//��� ��ȯ�� �ﰢ ���ۺ����� ���� fflushȣ��(�̰��� ������ �� ���� ����) 
		fgets(message, BUF_SIZE, readfp);//"r"���� ���� FILE*�� message���� readfp���� ǥ�� ���. 
		printf("Message from server: %s", message);
	}
	fclose(writefp);
	fclose(readfp);
	
	return 0;
} 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
