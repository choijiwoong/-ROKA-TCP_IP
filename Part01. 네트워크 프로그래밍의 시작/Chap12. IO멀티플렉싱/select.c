#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	fd_set reads, temps;//���ϵ�ũ���͸� ���� ���� 
	int result, str_len;
	char buf[BUF_SIZE];
	struct timeval timeout;//select�� timeout�� ������ ��(���ŷ ����) 
	
	FD_ZERO(&reads);//fd_set�� �ʱ�ȭ 
	FD_SET(0, &reads);//���� ��ũ���� 0�� 1�� �����Ѵ�. 
	
	/* while�ݺ��Ҷ����� ���µǰ� while�ȿ��� ����. 
	timeout.tv_sec=5;
	timeout.tv_usec=3000;
	*/
	
	while(1){
		temps=reads;//���� fd_set�� ���� ���� 
		timeout.tv_sec=5;//timeout�� ���� ���� 
		timeout.tv_usec=0;
		result=select(1, &temps, 0, 0, &timeout);//1���� ���� ��ũ����(����), fd_set(���ŵ��������翩�ο� �����ִٸ�), writeset(���ŷ�������ۿ���), exceptset(���ܿ���), timeout���� 
		if(result==-1){//�����߻��� 
			puts("select() error!");
			break;
		} else if(result==0){//timeout��(���ٸ� ��ȭ ����ä) 
			puts("Time-out!");
		} else{//��ȭ�� �߻��� 
			if(FD_ISSET(0, &temps)){//0�� ���� ��ũ���Ϳ� ��ȭ�� �߻��ߴ��� üũ ��  (ǥ�� �Է��� �´���) ����! ���ϵ�ũ���� 0�� stdin 1�� stdout 2�� stderr�ϱ�!!! 
				str_len=read(0, buf, BUF_SIZE);//��ȭ �߻��� ������ ���� ���� �� ���. 
				buf[str_len]=0;
				printf("message from console: %s", buf);
			}
		}
	}
	return 0;
}
