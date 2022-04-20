#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100 

void * thread_inc(void * arg);
void * thread_des(void * arg);
long long num=0;

int main(int argc, char *argv[]){//임계영역관점에서의 문제점을 확인하기 위해 오류가능성을 극대화시킨 코드 
	pthread_t thread_id[NUM_THREAD];
	int i;
	
	printf("sizeof long long: %d\n", sizeof(long long));
	for(i=0; i<NUM_THREAD; i++){//100개의 쓰레드 생성. 반은 50000000번 전역변수 num을 +1하고, 반은 -1한다. 
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
	}
	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);
			
	printf("result: %lld\n", num);//0이 아니다! 근데 이건 asm적으로 mov eax, value와 연산, 저장의 과정이 꼬여서 저장전에 mov하고 이지랄해서 그런거 
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
