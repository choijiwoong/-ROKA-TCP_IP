#include <stdio.h>
#include <fcntl.h>

int main(void){//fileno�� fdopen�Լ��� file descriptor�� FILE*��ȯ�� �Ͱ� �ݴ��, FILE*�� file descriptor�� ��ȯ�Ѵ�. 
	FILE *fp;
	int fd=open("data.dat", O_WRONLY|O_CREAT|O_TRUNC);//file descriptor�� ���� 
	if(fd==-1){
		fputs("file open error", stdout);
		return -1;
	}
	
	printf("First file descriptor: %d\n", fd);
	fp=fdopen(fd, "w");//file descriptor�� FILE *�� ��ȯ 
	fputs("TCP/IP SOCKET PROGRAMMING\n", fp);
	printf("Second file descriptor: %d\n", fileno(fp));//FILE*�� �ٽ� file descriptor�� ��ȯ. ��ȯ���� ���� ��ũ���Ͱ��� ��ȯ�� ���������� ���� ��ũ���Ͱ� ��ġ���� Ȯ���� �� �ִ�. 
	fclose(fp);
	
	return 0;
}
