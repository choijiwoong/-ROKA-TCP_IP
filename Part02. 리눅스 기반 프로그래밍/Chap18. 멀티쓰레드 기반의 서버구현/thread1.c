#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
void* thread_main(void *arg);

int main(int argc, char *argv[]){
	pthread_t t_id;
	int thread_param=5;
	
	if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0){//생성할 스레드ID저장을 위한 변수의 주소값, 쓰레드특성, 별도시작의 함수주소, 그 함수의 인자정보 
		puts("pthread_create() error");
		return -1;
	};
	sleep(10);
	puts("end of main");
	
	return 0;
} 

void* thread_main(void *arg){
	int i;
	int cnt=*((int*)arg);//int포인터로 바꾼 뒤 *로 값참조. 5 
	for(i=0; i<cnt; i++){
		sleep(1);
		//sleep(2)시 main이 그 사이에 종료하여 프로세스 전체가 소멸하기에 running thread가 모두 출력되지는 않는다. 
		puts("running thread");
	}
	
	return NULL;
}
