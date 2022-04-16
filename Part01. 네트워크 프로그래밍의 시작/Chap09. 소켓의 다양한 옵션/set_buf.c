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
	
	sock=socket(PF_INET, SOCK_STREAM, 0);//�⺻ ������ �翬�� ���� �ɼ� ���õǸ�, �����ϳ��� SO_RCVBUF��SO_SNDBUFũ��! 
	state=setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, sizeof(rcv_buf));//setsockopt����Ͽ� SO_RCVBUF���� rcv_buf������ ���� 
	if(state)
		error_handling("setsockopt() error!");
		
	state=setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));//sendbufferũ�� rcv_buf�� ���� 
	if(state)
		error_handling("setsockopt() error!");
		
	len=sizeof(snd_buf);
	state=getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, &len);//�״��� get SO_SNDBUF�� 
	if(state)
		error_handling("getsockopt() error!");
		
	len=sizeof(rcv_buf);
	state=getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void*)&rcv_buf, &len);
	if(state)
		error_handling("getsockopt() error!");
	
	printf("Input buffer size: %d\n", rcv_buf);//����� ����¹��� ���� ���ð� ��� 
	printf("Output buffer size: %d\n", snd_buf);//������ 1024*3=3072�ʹ� �޸�, 6144�� ���õǾ��ִ�.
	//����� ���۴� ����� �߿��� �����̱⿡ �츮�� �����ϴ� opt value�� ������ �����ϴ� ���� �ƴ�, �����ϴ� ���̴�. ��� ��Ȯ�� ���� �ƴ����� ������� �ݿ��� ���� Ȯ���� �� �ִ�. 
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
