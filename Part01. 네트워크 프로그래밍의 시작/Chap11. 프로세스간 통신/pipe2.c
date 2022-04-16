#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	int fds[2];
	char str1[]="Who are you?";
	char str2[]="Thank you for your message";
	char buf[BUF_SIZE];
	pid_t pid;
	
	pipe(fds);//���Ա� 
	pid=fork();
	if(pid==0){//�ڽ� 
		write(fds[1], str1, sizeof(str1));//�Ա���� ���� 
		sleep(2);//�ּ�ó���ϸ� ����� ���� ���������� ����� �������� �߻�. ������������ �ܼ��� �������̱⶧���� �θ� ���� ƴ ���� �ڱⰡ ���� ������������ �ڱⰡ ���� �����͸� ��Եȴ�. 
		read(fds[0], buf, BUF_SIZE);//�ⱸ��� �б� 
		printf("Child proc output: %s\n", buf);
	}else{//�θ� 
		read(fds[0], buf, BUF_SIZE);//�ⱸ��� �б� 
		printf("Parent proc output: %s\n", buf);
		write(fds[1], str2, sizeof(str2));//�Ա���� ���� 
		sleep(3);
	}//��� ���α׷� ���� �帧�� �����ϰ� �������� ���� Ÿ�̹��� ��Ʈ���ؾ��ϴµ�, �ý��ۿ� ���� �޶����� �κ��� ũ�⿡ �̴� ���� �Ұ����ϴ�. 
	//��� ����� ��� �ÿ��� �������� 2�� �����Ͽ� ����ϸ� �ȴ�. 
	return 0;
}
