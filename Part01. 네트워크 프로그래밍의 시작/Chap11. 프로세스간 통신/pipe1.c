#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];//파이프의 출구, 파이프의 입구 순이다. 
	char str[]="Who are you?";
	char buf[BUF_SIZE];
	pid_t pid;
	
	pipe(fds);//int형 배열 2개를 넣으면, 입출력으로 사용할 수 있는 파일 디스크립터 2개를 넣어준다. 
	pid=fork();
	if(pid==0){
		write(fds[1], str, sizeof(str));//index 1을 이용해 쓰기를 하고 
	}else{
		read(fds[0], buf, BUF_SIZE);//index 0을 이용해 읽기를 한다. 
		puts(buf);
	}
	
	return 0;
}
