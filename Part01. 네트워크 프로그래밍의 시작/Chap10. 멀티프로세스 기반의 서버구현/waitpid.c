#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int status;
	pid_t pid=fork();
	
	if(pid==0){//fork�� �Ļ��� �ڽ��� ��� 15�� ��� �� 24��ȯ 
		sleep(15);
		return 24;
	} else{
		while(!waitpid(-1, &status, WNOHANG)){//pid_t pid ù��° ���ڷ� ���Ḧ Ȯ���ϰ����ϴ� �ڽ� ���μ����� ID�� ���޹޴µ�, -1�� ������ ������ �ڽ� ���μ����� ���Ḧ ��ٸ���(�Ϲ����� wait�Լ��� ����) 
			sleep(3);
			puts("sleep 3sec.");
		}//���� �ڽ��� ���Ḧ 3�ʴ����� üũ. �Ϲ����� wait�Լ��� ��ȯ�ڽ��� ������ blocking ���°� �Ǵ� �ݸ鿡, WNOHANG�ɼ��� �ָ� ��ȯ�ڽ��� ���� ��� 0�� �����Ѵ�. 
		
		if(WIFEXITED(status))//���������� ���ᰡ �Ǿ����� üũ 
			printf("Child send %d\n", WEXITSTATUS(status));//��ȯ�� Ȯ�� 
	}
	return 0;
}
