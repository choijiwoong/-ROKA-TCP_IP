#include <stdio.h>
#include <windows.h>
#include <process.h>
unsigned WINAPI Read(void * arg);
unsigned WINAPI Accu(void * arg);

static HANDLE semOne;
static HANDLE semTwo;
static int num;

int main(int argc, char *argv[]) {
	HANDLE hThread1, hThread2;
	semOne=CreateSemaphore(NULL, 0, 1, NULL);//보안옵션, 초기값, 최대 세마포어값, 이름
	semTwo=CreateSemaphore(NULL, 1, 1, NULL);
	
	hThread1=(HANDLE)_beginthreadex(NULL, 0, Read, NULL, 0, NULL);
	hThread2=(HANDLE)_beginthreadex(NULL, 0, Accu, NULL, 0, NULL);
	
	WaitForSingleObject(hThread1, INFINITE);//쓰레드 마칠때까지 기달 
	WaitForSingleObject(hThread2, INFINITE);
	
	CloseHandle(semOne);//세마포어 종료. hMutex와 동일한 종료 함수. 
	CloseHandle(semTwo);
	
	return 0; 
}

unsigned WINAPI Read(void * arg){
	int i;
	for(i=0; i<5; i++){
		fputs("Input num: ", stdout);//입력받고 계산하고 하게 핑-퐁-핑-퐁 semaphore!
		WaitForSingleObject(semTwo, INFINITE);//hMutex와 동잃한 함수, 마찬가지의 release. 
		scanf("%d", &num);
		ReleaseSemaphore(semOne, 1, NULL);//2번째인자만큼 증가시켜서 release한다. 
	}
}

unsigned WINAPI Accu(void * arg){
	int sum=0, i;
	for(i=0; i<5; i++){
		WaitForSingleObject(semOne, INFINITE);
		sum+=num;
		ReleaseSemaphore(semTwo, 1, NULL);
	}
	printf("Result: %d\n", sum);
	return 0;
}
