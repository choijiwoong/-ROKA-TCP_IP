#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char *argv[]){
	struct iovec vec[2];
	char buf1[]="ABCDEFG";
	char buf2[]="1234567";
	int str_len;
	
	vec[0].iov_base=buf1;
	vec[0].iov_len=3;
	vec[1].iov_base=buf2;
	vec[1].iov_len=4;
	
	str_len=writev(1, vec, 2);//파일디스크립터1(stdout)에 vec(iovec)배열 2개를 한꺼번에 write한다. 
	puts("");
	printf("Write bytes: %d\n", str_len);
	return 0;
}
