#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];
	char str1[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;
	
	pipe(fds);//출입구 
	pid=fork();
	if(pid==0){//자식 
		write(fds[1], str1, sizeof(str1));//입구사용 쓰기 
		sleep(2);//주석처리하면 양방향 단일 파이프라인 사용의 문제점이 발생. 파이프라인은 단순히 선착순이기때문에 부모가 읽을 틈 없이 자기가 쓰고 가져가버리면 자기가 보낸 데이터를 얻게된다. 
		read(fds[0], buf, BUF_SIZE);//출구사용 읽기 
		printf("Child proc output: %s\n", buf);
	}else{//부모 
		read(fds[0], buf, BUF_SIZE);//출구사용 읽기 
		printf("Parent proc output: %s\n", buf);
		write(fds[1], str2, sizeof(str2));//입구사용 쓰기 
		sleep(3);
	}//고로 프로그램 실행 흐름을 예측하고 가져오고 쓰는 타이밍을 컨트롤해야하는데, 시스템에 따라 달라지는 부분이 크기에 이는 예측 불가능하다. 
	//고로 양방향 통신 시에는 파이프를 2개 생성하여 사용하면 된다. 
	return 0;
}
