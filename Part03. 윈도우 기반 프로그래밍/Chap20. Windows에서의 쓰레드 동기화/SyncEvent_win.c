#include <stdio.h>
#include <windows.h>
#include <process.h>
#define STR_LEN 100

unsigned WINAPI NumberOfA(void *arg);
unsigned WINAPI NumberOfOthers(void *arg);

static char str[STR_LEN];
static HANDLE hEvent;

int main(int argc, char *argv[]){//이전의 커널모드 오브젝트들과는 다르게 Event는 manual mode와 auto-reset mode를 설정할 수 있다. 
	HANDLE hThread1, hThread2;
	hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);//Event(2번째 인자가 TRUE면 manual mode, FALSE면 auto-reset mode) 
	hThread1=(HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);//쓰레드 생성. Event가 non-signaled이기에 wait상태. 
	hThread2=(HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);
	
	fputs("Input string: ", stdout);
	fgets(str, STR_LEN, stdin);//stdin에서 입력받고, SetEvent로 signal로 바꾸어 wait상태를 탈출시켜 쓰레드가 일제히 작업을 진행한다. 
	SetEvent(hEvent);//manual모드로 Event를 만들었기에, SetEvent로 signal을 TRUE로 세팅. (임계영역..?) 
	
	WaitForSingleObject(hThread1, INFINITE);//마칠때 까지 wait 
	WaitForSingleObject(hThread2, INFINITE);
	ResetEvent(hEvent);//signal을 FALSE로 세팅 
	CloseHandle(hEvent);
	
	return 0;
}

unsigned WINAPI NumberOfA(void *arg){//argument에서 A가 나온 수 카운트 및 출력 
	int i, cnt=0;
	WaitForSingleObject(hEvent, INFINITE);
	for(i=0; str[i]!=0; i++)
		if(str[i]=='A')
			cnt++;
	printf("Num of A: %d\n", cnt);
	return 0;
}

unsigned WINAPI NumberOfOthers(void *arg){//argument에서 A가 아닌 수 카운트 및 출력 
	int i, cnt=0;
	WaitForSingleObject(hEvent, INFINITE);
	for(i=0; str[i]!=0; i++)
		if(str[i]!='A')
			cnt++;
	printf("Num of others: %d\n", cnt-1);
	return 0;
}
