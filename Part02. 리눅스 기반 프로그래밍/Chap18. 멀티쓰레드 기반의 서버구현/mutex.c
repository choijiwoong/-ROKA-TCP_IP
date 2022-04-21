#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#define NUM_THREAD 100
void * thread_inc(void *arg);
void * thread_des(void * arg);

long long num=0;
pthread_mutex_t mutex;//뮤텍스 id를 저장할 변수 

int main(int argc, char *argv[]){
	pthread_t thread_id[NUM_THREAD];//스레드 아이디를 배열에 저장 
	int i;
	
	pthread_mutex_init(&mutex, NULL);//뮤텍스를 기본 설정으로 생성. PTHREAD_MUTEX_INITIALIZER로 대체 가능 
	
	for(i=0; i<NUM_THREAD; i++){
		if(i%2)
			pthread_create(&(thread_id[i]), NULL, thread_inc, NULL);
		else
			pthread_create(&(thread_id[i]), NULL, thread_des, NULL);
	}
	
	for(i=0; i<NUM_THREAD; i++)
		pthread_join(thread_id[i], NULL);
		
	printf("result: %lld\n", num);
	pthread_mutex_destroy(&mutex);//뮤텍스 파괴(해제) 
	return 0;
}

void * thread_inc(void * arg){
	int i;
	pthread_mutex_lock(&mutex);//critical section설정 
	for(i=0; i<5000000; i++)
		num+=1;
	pthread_mutex_unlock(&mutex);//critical section해제 
	return NULL;
}

void * thread_des(void * arg){
	int i;
	for(i=0; i<5000000; i++){
		pthread_mutex_lock(&mutex);
		num-=1;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}
