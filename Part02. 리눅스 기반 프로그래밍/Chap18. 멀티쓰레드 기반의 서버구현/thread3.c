#include <stdio.h>
#include <pthread.h>
void * thread_summation(void * arg);//암구호 
int sum=0;//두개의 쓰레드가 하나의 전역변수에 직접 접근이 가능한 것은 쓰레드는 데이터영역과 힙영영은 공유하되, 스택영역만 독립되어 있기 때문이다. 

int main(int argc, char *argv[]) {//worker thread model
	pthread_t id_t1, id_t2;
	int range1[]={1,5};//1부터 10까지의 합을 구하는데 2개의 쓰레드에 작업을 나누어 처리할 예정. 워커 쓰레드 모델이라고 부른다. 
	int range2[]={6,10};
	
	pthread_create(&id_t1, NULL, thread_summation, (void *)range1);
	pthread_create(&id_t2, NULL, thread_summation, (void *)range2);
	
	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	printf("result: %d\n", sum);
	
	return 0;
}

void * thread_summation(void * arg){
	int start=((int*)arg)[0];
	int end=((int*)arg)[1];
	
	while(start<=end){
		sum+=start;//전역변수 사용 in 데이터영역 
		start++;
	}
	return NULL;
}
