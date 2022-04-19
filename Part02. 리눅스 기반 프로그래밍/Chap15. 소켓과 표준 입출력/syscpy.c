#include <stdio.h>
#include <fcntl.h>
#define BUF_SIZE 3

int main(int argc, char *argv[]){//표준입출력함수를 이용할 경우 얻는 추가적인 버퍼로 인한 성능차이 비교를 위함. 
	int fd1, fd2;
	int len;
	char buf[BUF_SIZE];
	
	fd1=open("news.txt", O_RDONLY);
	fd2=open("cpy.txt", O_WRONLY|O_CREAT_O_TRUNC);
	
	while((len=read(fd1, buf, sizeof(buf)))>0)
		write(fd2, buf, len);//TCP에 사용하는 함수. 재전송을 위한 버퍼의 사용. 
		
	close(fd1);
	close(fd2);
	return 0;
}
