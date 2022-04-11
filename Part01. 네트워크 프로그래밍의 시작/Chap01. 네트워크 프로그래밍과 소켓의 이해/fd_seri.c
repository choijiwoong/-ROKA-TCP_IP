//파일 생성, 소켓 생성 후 반환되는 파일 디스크립터의 값을 정수형태로 비교 
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

int main(void){
	int fd1, fd2, fd3;
	fd1=socket(PF_INET, SOCK_STREAM, 0);
	fd2=open("test.dat", O_CREAT|O_WRONLY|O_TRUNC);
	fd3=socket(PF_INET, SOCK_DGRAM, 0);
	
	printf("file descriptor 1: %d\n", fd1);//3 0은 stdinput, 1은 stdoutput, 2는 stderr에 이미 할당되어 있기에 3부터 할당된다. 
	printf("file descriptor 2: %d\n", fd2);//4
	printf("file descriptor 3: %d\n", fd3);//5
	
	close(fd1); close(fd2); close(fd3);
	
	return 0;
}
