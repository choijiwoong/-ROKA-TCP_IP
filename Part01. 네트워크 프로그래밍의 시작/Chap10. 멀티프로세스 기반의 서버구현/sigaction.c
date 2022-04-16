#include <stdio.h>
#include <unistd.h>
#include <signal.h> 

void timeout(int sig){
	if(sig==SIGALRM)
		puts("Time out!");
	alarm(2);
}

int main(int argc, char *argv[]){
	int i;
	struct sigaction act;//for use sigaction not signal function
	act.sa_handler=timeout;//ȣ��� �Լ� ���� 
	sigemptyset(&act.sa_mask);//��� 0���� �ʱ�ȭ
	act.sa_flags=0;//0���� �ʱ�ȭ 
	sigaction(SIGALRM, &act, 0);//SIGALARM�߻��� act�� ȣ���ϰԲ� sigaction function�� �̿�. 3��° ���ڴ� old_function�� ��ȯ���� ������ ��Ÿ����. 
	//signal�Լ����� �ü�� ȣȯ�� �鿡�� �������̴�.	
	alarm(2);//SIGALRM
	
	for(i=0; i<3; i++){
		puts("wait...");//wait�߿��� wake�Ͽ� act.sa_hanlder�� ����� timeout�Լ� ȣ��. 
		sleep(100);
	}
	return 0;
}
