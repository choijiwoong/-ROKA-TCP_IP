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
	sem_init(&sem_one, 0, 0);//�ʱⰪ 0 
	sem_init(&sem_two, 0, 1);//�ʱⰪ 1
	
	pthread_create(&id_t1, NULL, read, NULL);//5���� �Է��� ���� num�� ���� 
	pthread_create(&id_t2, NULL, accu, NULL);//num�� sum�� ���Ͽ� �������� ��� 
	
	pthread_join(id_t1, NULL);
	pthread_join(id_t2, NULL);
	
	sem_destroy(&sem_one);//�������� ���� 
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
}//�ʱⰪ�� sem2�� Ȱ��ȭ�Ǿ��ֱ⿡ read���� ����Ǿ� scanf�� �� sem_one�� Ȱ��ȭ���� ������ �Ѵ�. ��, �������� �ϳ��� �Է��� ������ �����ϴ� ���°� �ǵ��� �������� 2���� �̿��Ѵ�(�ʱⰪ�� ��������)
//���� ���ؼ� accuȰ��ȭ�� sem1��, readȰ��ȭ�� sem2�� ����Ѵ�. 
