#include <stdio.h>
#include <fcntl.h>

int main(void){//���� ��ũ���� ������� FILE�����͸� �� ǥ�� ����� �Լ��� ����� �� �ִ�. 
	FILE *fp;
	int fd=open("data.data", O_WRONLY|O_CREAT|O_TRUNC);//���������� �Ϲ� open���� ��� file_descriptor�� ��´�. 
	if(fd==-1){
		fputs("file open error", stdout);
		return -1;
	}
	
	fp=fdopen(fd, "w");//fdopen�� �̿��Ͽ� file descriptor�� FILE*�� ��ȯ�Ѵ�. 
	fputs("Network C programming\n", fp);//FILE* ǥ��������Լ��� ����ϰ� ���������� �ݴ�(close)��. 
	fclose(fp);
	
	return 0;
} 
