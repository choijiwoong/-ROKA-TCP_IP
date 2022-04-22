#include <stdio.h>
#include <windows.h>//Ŀ�θ���� ����ȭ ��� �� �ϳ��̱⿡ <windows.h>�� �����Ѵ�. 
#include <process.h>

#define NUM_THREAD 50
unsigned WINAPI threadInc(void * arg);
unsigned WINAPI threadDes(void * arg);

long long num=0;
HANDLE hMutex;//Mutex�� HANDLE ����  

int main(int argc, char *argv[]){
	HANDLE tHandles[NUM_THREAD];
	int i;
	
	hMutex=CreateMutex(NULL, FALSE, NULL);//���ȿɼ�, �����ھ��� Mutex������Ʈ signaled���·� ����, �̸� NULl 
	for(i=0; i<NUM_THREAD; i++){
		if(i%2)
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
		else
			tHandles[i]=(HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
	}
	
	WaitForMultipleObjects(NUM_THREAD, tHandles, TRUE, INFINITE);//���� �����带 ALL�ɼ����� wait 
	CloseHandle(hMutex);//hMutex ���� 
	printf("result: %lld\n", num);
	
	return 0;
}

unsigned WINAPI threadInc(void * arg){
	int i;
	WaitForSingleObject(hMutex, INFINITE);//WaitForSingleObject�Լ��� �̿��Ͽ� hMutex�� lock. �� �Լ��� ������ ������ ��ȯ���� ��ٸ��� ����߾���. 
	for(i=0; i<5000000; i++)
		num+=1;
	ReleaseMutex(hMutex);//ReleaseMutex�� hMutex unlock 
	
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
