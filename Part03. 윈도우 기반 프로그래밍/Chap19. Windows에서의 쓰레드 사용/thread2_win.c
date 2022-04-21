#include <stdio.h>
#include <windows.h>
#include <process.h>
unsigned WINAPI ThreadFunc(void *arg);

int main(int argc, char *argv[]){
	HANDLE hThread;
	DWORD wr;
	unsigned threadID;
	int param=5;
	
	hThread=(HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);//_beginthread함수와 달리 커널 오브젝트에 접근이 가능하다! 
	if(hThread==0){
		puts("_beginthreadex() error");
		return -1;
	}
	
	if((wr=WaitForSingleObject(hThread, INFINITE))==WAIT_FAILED){//timeout을 INFINITE로 할 시 반환까지 blocking. 
		puts("thread wait error");//WAIT_OBJECT_0 혹은 WAIT_TIMEOUT반환 
		return -1;
	}
	
	printf("wait result: %s\n", (wr==WAIT_OBJECT_0)? "signaled": "time-out");//signaled로 인한 상태 반환시 WAIT_OBJECT_0을 반환 
	puts("end of main");
	return 0;
}

unsigned WINAPI ThreadFunc(void * arg){
	int i;
	int cnt=*((int*)arg);
	for(i=0; i<cnt; i++){//5초후 반환. 
		Sleep(1000);
		puts("running thread");
	}
	return 0;
}
