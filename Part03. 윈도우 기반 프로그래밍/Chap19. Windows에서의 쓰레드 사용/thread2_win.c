#include <stdio.h>
#include <windows.h>
#include <process.h>
unsigned WINAPI ThreadFunc(void *arg);

int main(int argc, char *argv[]){
	HANDLE hThread;
	DWORD wr;
	unsigned threadID;
	int param=5;
	
	hThread=(HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);//_beginthread�Լ��� �޸� Ŀ�� ������Ʈ�� ������ �����ϴ�! 
	if(hThread==0){
		puts("_beginthreadex() error");
		return -1;
	}
	
	if((wr=WaitForSingleObject(hThread, INFINITE))==WAIT_FAILED){//timeout�� INFINITE�� �� �� ��ȯ���� blocking. 
		puts("thread wait error");//WAIT_OBJECT_0 Ȥ�� WAIT_TIMEOUT��ȯ 
		return -1;
	}
	
	printf("wait result: %s\n", (wr==WAIT_OBJECT_0)? "signaled": "time-out");//signaled�� ���� ���� ��ȯ�� WAIT_OBJECT_0�� ��ȯ 
	puts("end of main");
	return 0;
}

unsigned WINAPI ThreadFunc(void * arg){
	int i;
	int cnt=*((int*)arg);
	for(i=0; i<cnt; i++){//5���� ��ȯ. 
		Sleep(1000);
		puts("running thread");
	}
	return 0;
}
