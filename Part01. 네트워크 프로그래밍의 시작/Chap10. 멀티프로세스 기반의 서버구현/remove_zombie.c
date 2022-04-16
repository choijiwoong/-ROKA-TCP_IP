#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void read_childproc(int sig){//�ñ׳� �߻��� ȣ��Ǵ� �Լ��� 
	int status;
	pid_t id=waitpid(-1, &status, WNOHANG);//waitpid�� ������¸� üũ�Ѵ�. 
	if(WIFEXITED(status)){
		printf("Removed proc id: %d\n", id);
		printf("Child send: %d\n", WEXITSTATUS(status));
	}
}

int main(int argc, char *argv[]){
	pid_t pid;
	struct sigaction act;//for sigaction function
	act.sa_handler=read_childproc;//callback�Լ� 
	sigemptyset(&act.sa_mask);//0���� �ʱ�ȭ 
	act.sa_flags=0;
	sigaction(SIGCHLD, &act, 0);//�ڽ� ���μ��� ����� signal�߻��Ͽ� parent ���μ����� wakeup 
	
	pid=fork();
	if(pid==0){//�ڽ��� ��� 10���� 12���� 
		puts("Hi! I'm child process'");
		sleep(10);
		return 12;
	}else{
		printf("Child proc id: %d\n", pid);
		pid=fork();//�ڽ� �ϳ� �� ���� 
		if(pid==0){//�ڽ��� 10ȣ�� 24���� 
			puts("Hi! I'm child process");
			sleep(10);
			exit(24);
		}else{
			int i;
			printf("Child proc id: %d \n", pid);//�θ�� child process�� pid�� ����ϰ� sleep�һ�.. ���߿� �ñ׳� �߻� �� wakeup�Ͽ� callbackȣ�⿹�� 
			for(i=0; i<5; i++){
				puts("wait...");
				sleep(5);
			}
		}
	} 
}
