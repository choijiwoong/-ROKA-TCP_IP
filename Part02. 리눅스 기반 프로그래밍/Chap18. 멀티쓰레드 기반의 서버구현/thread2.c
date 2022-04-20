#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[]){
	pthread_t t_id;
	int thread_param=5;
	void * thr_ret;
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0){//생성된 스레드 주소를 t_id에 저장, thread_main함수를 thread_param을 인자로 쓰레드 생성 
		puts("pthread_create() error");
		return -1;
	}
	
	if(pthread_join(t_id, &thr_ret)!=0){//thread가 종료될 떄까지 기다린다. 반환받은 값을 thr_ret에 저장한다. 
		puts("pthread_join() error");
		return -1;
	}
	
	printf("Thread return message: %s\n", (char*)thr_ret);//반환받은 값을 형변환을 통해 출력한다. 
	free(thr_ret);
	
	return 0;
}

void* thread_main(void *arg){//5
	int i;
	int cnt=*((int*)arg);//값참조 
	char * msg=(char *)malloc(sizeof(char)*50);//동적할당 
	strcpy(msg, "Hello, I'am thread~\n"); 
	
	for(i=0; i<cnt; i++){
		sleep(1);
		puts("running thread");
	}
	return (void*)msg;//메시지 리턴
}
