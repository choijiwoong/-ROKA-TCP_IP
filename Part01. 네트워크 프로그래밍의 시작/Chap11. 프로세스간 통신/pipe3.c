#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds1[2], fds2[2];
	char str1[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;
	
	pipe(fds1), pipe(fds2);
	pid=fork();
	if(pid==0){
		write(fds1[1], str1, sizeof(str1));//1입구에 하나 쓰고 
		read(fds2[0], buf, BUF_SIZE);//2 출구로 하나 받고 
		printf("Child proc output: %s\n", buf);
	} else{
		read(fds1[0], buf, BUF_SIZE);//1출구로 하나 읽고 
		printf("Parent proc output: %s\n", buf);
		write(fds2[1], str2, sizeof(str2));//2입구로 하나 넣고 
		sleep(3);
	}
	
	return 0;
}
