#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
void error_handling(char* message);

int main(void){//리눅스는 파일과 소켓을 동일시하게 생각한다! 고로 이전에 write로 소켓 전송함. 파일디스크립터는 윈도우 핸들개념 
	int fd;
	char buf[]="Let's go!\n";
	
	fd=open("data.txt", O_CREAT|O_WRONLY|O_TRUNC);//필요시 생성, 쓰기전용, 기존 데이터 삭제
	if(fd==-1)
		error_handling("open() error!");
	printf("file descriptor: %d \n", fd) ;
	
	if(write(fd, buf, sizeof(buf))==-1)//fd의 내용을 buf에 write 
		error_handling("write() error!");
	close(fd);
	return 0;
}
void error_handling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
