#include <stdio.h>
#include <fcntl.h>

int main(void){//파일 디스크립터 기반으로 FILE포인터를 얻어내 표준 입출력 함수를 사용할 수 있다. 
	FILE *fp;
	int fd=open("data.data", O_WRONLY|O_CREAT|O_TRUNC);//실행파일을 일반 open으로 열어서 file_descriptor를 얻는다. 
	if(fd==-1){
		fputs("file open error", stdout);
		return -1;
	}
	
	fp=fdopen(fd, "w");//fdopen을 이용하여 file descriptor를 FILE*로 변환한다. 
	fputs("Network C programming\n", fp);//FILE* 표준입출력함수로 사용하고 마찬가지로 닫는(close)다. 
	fclose(fp);
	
	return 0;
} 
