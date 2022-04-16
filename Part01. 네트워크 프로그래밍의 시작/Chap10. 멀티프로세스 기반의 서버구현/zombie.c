#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	pid_t pid=fork();
	
	if(pid==0){
		puts("Hi, I am a child process");
	}else{
		printf("Child Process Id: %d\n", pid);
		sleep(30);
	}
	
	if(pid==0)
		puts("End child process");//완전한 종료를 위해서는 Parent에게 반환값을 전달해야하는데 현재 parent가 sleep상태이기에 daemon이 된다. 
	else
		puts("End parent process");
		
	return 0;
}
