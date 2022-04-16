#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int status;
	pid_t pid=fork();//parent, child
	
	if(pid==0){
		return 3;//child end
	} else{
		printf("Child PID: %d\n", pid);
		pid=fork();//parent, child2
		if(pid==0){
			exit(7);//child end
		} else{
			printf("Child PID: %d\n", pid);
			wait(&status);
			if(WIFEXITED(status))//wait if exited, wait exit status(반환값) 
				printf("Child send one: %d\n", WEXITSTATUS(status));//child's return. 차례로 접근 가능한듯.  
			
			wait(&status);
			if(WIFEXITED(status))
				printf("Child send two: %d\n", WEXITSTATUS(status));//child2's return
			sleep(30);
		}
	}
	return 0;
} 
