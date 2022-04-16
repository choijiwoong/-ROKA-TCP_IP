#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig){
	if(sig==SIGALRM)
		puts("Time out!");
	alarm(2);//signal.h내부 함수로 해당 초가 지나면 SIGALRM시그널을 발생시킨다. 
} 
void keycontrol(int sig){
	if(sig==SIGINT)
		puts("CTRL+C pressed");
}

int main(int argc, char *argv[]){
	int i;
	signal(SIGALRM, timeout);//SIGALRM 시그널 발생 시 timeout함수를 호출 
	signal(SIGINT, keycontrol);//SIGINT(Ctrl+C) 시그널 발생 시 keycontrol함수를 호출 
	alarm(2);//2초 후 SIGALRM시그널 발생 
	
	for(i=0; i<3; i++){
		puts("wait...");
		sleep(100);//프로세스가 잠든 상태(블로킹?)에서 alarm으로 인한 SIGALRM 혹은 Ctrl+C로 인한 SIGINT 시그널 호출 시 프로세스가 깨어난다. 
	}
	
	return 0;
}
