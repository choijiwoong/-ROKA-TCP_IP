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
	act.sa_handler=timeout;//호출되 함수 지정 
	sigemptyset(&act.sa_mask);//모두 0으로 초기화
	act.sa_flags=0;//0으로 초기화 
	sigaction(SIGALRM, &act, 0);//SIGALARM발생시 act를 호출하게끔 sigaction function을 이용. 3번째 인자는 old_function을 반환할지 말지를 나타낸다. 
	//signal함수보다 운영체제 호환성 면에서 안정적이다.	
	alarm(2);//SIGALRM
	
	for(i=0; i<3; i++){
		puts("wait...");//wait중에서 wake하여 act.sa_hanlder에 저장된 timeout함수 호출. 
		sleep(100);
	}
	return 0;
}
