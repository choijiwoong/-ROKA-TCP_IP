#include <stdio.h>
#include <fcntl.h>

int main(void){//fileno는 fdopen함수가 file descriptor를 FILE*변환한 것과 반대로, FILE*을 file descriptor로 변환한다. 
	FILE *fp;
	int fd=open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);//file descriptor로 오픈 
	if(fd==-1){
		fputs("file open error", stdout);
		return -1;
	}
	
	printf("First file descriptor: %d\n", fd);
	fp=fdopen(fd, "w");//file descriptor를 FILE *로 변환 
	fputs("TCP/IP SOCKET PROGRAMMING\n", fp);
	printf("Second file descriptor: %d\n", fileno(fp));//FILE*를 다시 file descriptor로 변환. 변환전의 파일 디스크립터값과 변환후 복구이후의 파일 디스크립터가 일치함을 확인할 수 있다. 
	fclose(fp);
	
	return 0;
}
