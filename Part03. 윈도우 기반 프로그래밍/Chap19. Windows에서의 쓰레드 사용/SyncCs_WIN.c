#include <stdio.h>
#include <windows.h>
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void * arg);
unsigned WINAPI threadDes(void * arg);

long long num=0;
CRITICAL_SECTION cs;//�������� User mode�� Mutex 

int main(int argc, char *argv[]){//[��������� Lock, unlock�� ���Ǵ� SRITICAL_SECTION] 
	HANDLE tHandles[NUM_THREAD];
	int i;
	
	InitializeCriticalSection(&cs);//������� CRITICAL_SECTION���� 
	for(i=0; i<NUM_THREAD; i++){
		if(i%2)
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
		else
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
	}
	
	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);
	DeleteCriticalSection(&cs);
	printf("result: %lld\n", num);
	
	return 0;
}

unsigned WINAPI threadInc(void * arg){
	int i;
	EnterCriticalSection(&cs);//Lockó�� ���. ���� �������. 
	for(i=0; i<5000000; i++)
		num+=1;
	LeaveCriticalSection(&cs);
	
	return 0;
}

unsigned WINAPI threadDes(void * arg){
	int i;
	EnterCriticalSection(&cs);
	for(i=0; i<5000000; i++)
		num-=1;
	LeaveCriticalSection(&cs);
	
	return 0;
}
