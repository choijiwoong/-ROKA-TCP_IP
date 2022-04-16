#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];//�������� �ⱸ, �������� �Ա� ���̴�. 
	char str[]="Who are you?";
	char buf[BUF_SIZE];
	pid_t pid;
	
	pipe(fds);//int�� �迭 2���� ������, ��������� ����� �� �ִ� ���� ��ũ���� 2���� �־��ش�. 
	pid=fork();
	if(pid==0){
		write(fds[1], str, sizeof(str));//index 1�� �̿��� ���⸦ �ϰ� 
	}else{
		read(fds[0], buf, BUF_SIZE);//index 0�� �̿��� �б⸦ �Ѵ�. 
		puts(buf);
	}
	
	return 0;
}
