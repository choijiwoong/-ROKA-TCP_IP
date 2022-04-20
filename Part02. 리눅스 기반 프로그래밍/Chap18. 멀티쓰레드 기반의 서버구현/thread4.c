#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100 

void * thread_inc(void * arg);
void * thread_des(void * arg);
long long num=0;

int main(int argc, char *argv[]){//�Ӱ迵������������ �������� Ȯ���ϱ� ���� �������ɼ��� �ش�ȭ��Ų �ڵ� 
	pthread_t thread_id[NUM_THREAD];
	int i;
	
	printf("sizeof long long: %d\n", sizeof(long long));
	for(i=0; i<NUM_THREAD; i++){//100���� ������ ����. ���� 50000000�� �������� num�� +1�ϰ�, ���� -1�Ѵ�. 
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
	}
	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);
			
	printf("result: %lld\n", num);//0�� �ƴϴ�! �ٵ� �̰� asm������ mov eax, value�� ����, ������ ������ ������ �������� mov�ϰ� �������ؼ� �׷��� 
	return 0;
}

void * thread_inc(void * arg){
	int i;
	for(i=0; i<50000000; i++)
		num+=1;
	return NULL;
}

void * thread_des(void * arg){
	int i;
	for(i=0; i<50000000; i++)
		num-=1;
	return NULL;
}
