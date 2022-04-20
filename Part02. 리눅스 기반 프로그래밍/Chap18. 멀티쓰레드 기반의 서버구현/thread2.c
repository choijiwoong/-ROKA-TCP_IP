#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[]){
	pthread_t t_id;
	int thread_param=5;
	void * thr_ret;
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0){//������ ������ �ּҸ� t_id�� ����, thread_main�Լ��� thread_param�� ���ڷ� ������ ���� 
		puts("pthread_create() error");
		return -1;
	}
	
	if(pthread_join(t_id, &thr_ret)!=0){//thread�� ����� ������ ��ٸ���. ��ȯ���� ���� thr_ret�� �����Ѵ�. 
		puts("pthread_join() error");
		return -1;
	}
	
	printf("Thread return message: %s\n", (char*)thr_ret);//��ȯ���� ���� ����ȯ�� ���� ����Ѵ�. 
	free(thr_ret);
	
	return 0;
}

void* thread_main(void *arg){//5
	int i;
	int cnt=*((int*)arg);//������ 
	char * msg=(char *)malloc(sizeof(char)*50);//�����Ҵ� 
	strcpy(msg, "Hello, I'am thread~\n"); 
	
	for(i=0; i<cnt; i++){
		sleep(1);
		puts("running thread");
	}
	return (void*)msg;//�޽��� ����
}
