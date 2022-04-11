#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 100
void error_handling(char* message);

int main(void){
	int fd;
	char buf[BUF_SIZE];
	
	fd=open("data.txt", O_RDONLY);//읽기 전용으로 
	if(fd==-1)
		error_handling("open() error!");
	printf("file descriptor: %d\n", fd);//파일 디스크립터(핸들) 3 
	
	if(read(fd, buf, sizeof(buf))==-1)//buf에 읽기 
		error_handling("read() error!");
	printf("file data: %s", buf);//출력 
	close(fd);
	return 0;
}

void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
