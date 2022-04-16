#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
	if(sig==SIGALRM)
		puts("Time out!");
	alarm(2);//signal.h���� �Լ��� �ش� �ʰ� ������ SIGALRM�ñ׳��� �߻���Ų��. 
} 
void keycontrol(int sig){
	if(sig==SIGINT)
		puts("CTRL+C pressed");
}

int main(int argc, char *argv[]){
	int i;
	signal(SIGALRM, timeout);//SIGALRM �ñ׳� �߻� �� timeout�Լ��� ȣ�� 
	signal(SIGINT, keycontrol);//SIGINT(Ctrl+C) �ñ׳� �߻� �� keycontrol�Լ��� ȣ�� 
	alarm(2);//2�� �� SIGALRM�ñ׳� �߻� 
	
	for(i=0; i<3; i++){
		puts("wait...");
		sleep(100);//���μ����� ��� ����(���ŷ?)���� alarm���� ���� SIGALRM Ȥ�� Ctrl+C�� ���� SIGINT �ñ׳� ȣ�� �� ���μ����� �����. 
	}
	
	return 0;
}
