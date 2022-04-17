#include <stdio.h>
#include <sys/uio.h>
#define BUF_SIZE 100

int main(int argc, char *argv[]){
	struct iovec vec[2];
	char buf1[BUF_SIZE]={0,};
	char buf2[BUF_SIZE]={0,};
	int str_len;
	
	vec[0].iov_base=buf1;
	vec[0].iov_len=5;
	vec[1].iov_base=buf2;
	vec[1].iov_len=BUF_SIZE;
	
	str_len=readv(0, vec, 2);//순차적으로 잘려서 들어간다! 즉 buf1에 앞에 5글자, buf2에 남은 글자가 저장된다. 
	printf("Read bytes: %d\n", str_len);//readv를 통해 수신한 메시지를 여러 버퍼에 나눠 한꺼번에 담을 수 있다. 
	printf("First message: %s\n", buf1);
	printf("Second message: %s\n", buf2);
	
	return 0;
}
