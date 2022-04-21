#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

void * read(void * arg);
void * accu(void * arg);
static sem_t sem_one;
static sem_t sem_two;
static int num;

int main(in argc, char *argv[]){//binary semaphore!
	pthread_t id_t1, id_t2;
	sem_init(&sem_one, 0, 0);//초기값 0 
	sem_init(&sem_two, 0, 1);//초기값 1
	
	pthread_create(&id_t1, NULL, read, NULL);//5개의 입력을 맏아 num에 저장 
	pthread_create(&id_t2, NULL, accu, NULL);//num을 sum에 더하여 누산결과를 출력 
	
	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	
	sem_destroy(&sem_one);//세마포어 해제 
	sem_destroy(&sem_two);
	
	return 0;
}

void * read(void * arg){
	int i;
	for(i=0; i<5; i++){
		fputs("Input num: ", stdout);
		
		sem_wait(&sem_two);//sem2_val=0
		scanf("%d", &num);
		sem_post(&sem_one);//sem1_val=1
	}
	return NULL;
}
void * accu(void * arg){
	int sum=0, i;
	for(i=0; i<5; i++){
		sem_wait(&sem_one);//sem1_val=0
		sum+=num;
		sem_post(&sem_two);//sem2_val=1
	}
	printf("Result: %d\n", sum);
	return NULL;
}//초기값이 sem2가 활성화되어있기에 read부터 실행되어 scanf한 뒤 sem_one을 활성화시켜 누산을 한다. 즉, 차례차례 하나의 입력을 받으면 누산하는 형태가 되도록 세마포어 2개를 이용한다(초기값도 마찬가지)
//쉽게 말해서 accu활성화는 sem1이, read활성화는 sem2가 담당한다. 
