#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){//시그널 발생시 호출되는 함수로 
	int status;
	pid_t id=waitpid(-1, &status, WNOHANG);//waitpid즉 종료상태를 체크한다. 
	if(WIFEXITED(status)){
		printf("Removed proc id: %d\n", id);
		printf("Child send: %d\n", WEXITSTATUS(status));
	}
}

int main(int argc, char *argv[]){
	pid_t pid;
	struct sigaction act;//for sigaction function
	act.sa_handler=read_childproc;//callback함수 
	sigemptyset(&act.sa_mask);//0으로 초기화 
	act.sa_flags=0;
	sigaction(SIGCHLD, &act, 0);//자식 프로세스 종료시 signal발생하여 parent 프로세스를 wakeup 
	
	pid=fork();
	if(pid==0){//자식의 경우 10초후 12리턴 
		puts("Hi! I'm child process'");
		sleep(10);
		return 12;
	}else{
		printf("Child proc id: %d\n", pid);
		pid=fork();//자식 하나 더 생성 
		if(pid==0){//자식은 10호추 24리턴 
			puts("Hi! I'm child process");
			sleep(10);
			exit(24);
		}else{
			int i;
			printf("Child proc id: %d \n", pid);//부모는 child process의 pid를 출력하고 sleep할뿐.. 나중에 시그널 발생 시 wakeup하여 callback호출예정 
			for(i=0; i<5; i++){
				puts("wait...");
				sleep(5);
			}
		}
	} 
}
