#include <stdio.h>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void * arg);
unsigned WINAPI threadDes(void * arg);
long long num=0;

int main(int argc, char *argv[]){
	HANDLE tHandles[NUM_THREAD];
	int i;
	
	printf("sizeof long long: %d\n", sizeof(long long));
	for(i=0; i<NUM_THREAD; i++){
		if(i%2)
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
		else
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
	}
	
	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);//MultipleObject의 signal을 기다리는 함수를 호출. 3번쨰 인자가 1이면 ALL, 0이면 Any를 의미. 
	printf("result: %lld\n", num);//현재로서는 반환값이 매번 다르다.아마 critical section이 없어서 그럴듯. 
	
	return 0;
}

unsigned WINAPI threadInc(void * arg){
	int i;
	for(i=0; i<5000000; i++)
		num+=1;
		
	return 0;
}

unsigned WINAPI threadDes(void * arg){
	int i;
	for(i=0; i<5000000; i++)
		num-=1;
		
	return 0;
}
