#include <stdio.h>
#include <windows.h>
#include <process.h>
unsigned WINAPI ThreadFunc(void *arg);//�Ҵ�� ������ ��ȯ���, �Ű������� ���޹������ �����ϴ� �Լ��� ȣ��Ծ� ���. 

int main(int argc, char *argv[]){
	HANDLE hThread;//id�� HANDLE�� ����ȯ�Ͽ� ���� ���� 
	unsigned threadID;//id���� ���� 
	int param=5;
	
	hThread=(HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);//������ ������ �Լ��ּ�, �� ����, id���� ���� 
	if(hThread==0){
		puts("_beginthreadex() error");
		return -1;
	}
	Sleep(3000);
	puts("end of main");
	
	return 0;
}

unsigned WINAPI ThreadFunc(void *arg){
	int i;
	int cnt=*((int*)arg);
	for(i=0; i<cnt; i++){
		Sleep(1000);
		puts("running thread");
	}
	
	return 0;
}
