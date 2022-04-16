#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int status;
	pid_t pid=fork();
	
	if(pid==0){//fork로 파생된 자식의 경우 15초 대기 후 24반환 
		sleep(15);
		return 24;
	} else{
		while(!waitpid(-1, &status, WNOHANG)){//pid_t pid 첫번째 인자로 종료를 확인하고자하는 자식 프로세스의 ID를 전달받는데, -1로 설정시 임의의 자식 프로세스의 종료를 기다린다(일반적인 wait함수와 같이) 
			sleep(3);
			puts("sleep 3sec.");
		}//임의 자식의 종료를 3초단위로 체크. 일반적인 wait함수는 반환자식이 없으면 blocking 상태가 되는 반면에, WNOHANG옵션을 주면 반환자식이 없는 경우 0을 리턴한다. 
		
		if(WIFEXITED(status))//정상적으로 종료가 되었는지 체크 
			printf("Child send %d\n", WEXITSTATUS(status));//반환값 확인 
	}
	return 0;
}
