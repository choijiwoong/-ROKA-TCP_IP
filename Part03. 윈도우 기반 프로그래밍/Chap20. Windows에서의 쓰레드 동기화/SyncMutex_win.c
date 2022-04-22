#include <stdio.h>
#include <windows.h>//커널모드의 동기화 방법 중 하나이기에 <windows.h>를 포함한다. 
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void * arg);
unsigned WINAPI threadDes(void * arg);

long long num=0;
HANDLE hMutex;//Mutex용 HANDLE 변수  

int main(int argc, char *argv[]){
	HANDLE tHandles[NUM_THREAD];
	int i;
	
	hMutex=CreateMutex(NULL, FALSE, NULL);//보안옵션, 소유자없응 Mutex오브젝트 signaled상태로 설정, 이름 NULl 
	for(i=0; i<NUM_THREAD; i++){
		if(i%2)
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
		else
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
	}
	
	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);//여러 스레드를 ALL옵션으로 wait 
	CloseHandle(hMutex);//hMutex 해제 
	printf("result: %lld\n", num);
	
	return 0;
}

unsigned WINAPI threadInc(void * arg){
	int i;
	WaitForSingleObject(hMutex, INFINITE);//WaitForSingleObject함수를 이용하여 hMutex를 lock. 이 함수는 이전에 쓰레드 반환값을 기다릴때 사용했었다. 
	for(i=0; i<5000000; i++)
		num+=1;
	ReleaseMutex(hMutex);//ReleaseMutex로 hMutex unlock 
	
	return 0;
}

unsigned WINAPI threadDes(void * arg){
	int i;
	WaitForSingleObject(hMutex, INFINITE);
	for(i=0; i<5000000; i++)
		num-=1;
	ReleaseMutex(hMutex);
	
	return 0;
}
