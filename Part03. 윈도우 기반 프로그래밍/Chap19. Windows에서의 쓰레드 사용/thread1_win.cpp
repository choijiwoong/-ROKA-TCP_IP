#include <stdio.h>
#include <windows.h>
#include <process.h>
unsigned WINAPI ThreadFunc(void *arg);//할당된 스택의 반환방법, 매개변수의 전달방향등을 포함하는 함수의 호출규약 명시. 

int main(int argc, char *argv[]){
	HANDLE hThread;//id를 HANDLE로 형변환하여 담을 변수 
	unsigned threadID;//id담을 변수 
	int param=5;
	
	hThread=(HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);//쓰레드 시작할 함수주소, 들어갈 인자, id담을 변수 
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
