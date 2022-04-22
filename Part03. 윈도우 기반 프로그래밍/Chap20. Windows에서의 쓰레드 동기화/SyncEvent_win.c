#include <stdio.h>
#include <windows.h>
#include <process.h>
#define STR_LEN 100

unsigned WINAPI NumberOfA(void *arg);
unsigned WINAPI NumberOfOthers(void *arg);

static char str[STR_LEN];
static HANDLE hEvent;

int main(int argc, char *argv[]){//������ Ŀ�θ�� ������Ʈ����� �ٸ��� Event�� manual mode�� auto-reset mode�� ������ �� �ִ�. 
	HANDLE hThread1, hThread2;
	hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);//Event(2��° ���ڰ� TRUE�� manual mode, FALSE�� auto-reset mode) 
	hThread1=(HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);//������ ����. Event�� non-signaled�̱⿡ wait����. 
	hThread2=(HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);
	
	fputs("Input string: ", stdout);
	fgets(str, STR_LEN, stdin);//stdin���� �Է¹ް�, SetEvent�� signal�� �ٲپ� wait���¸� Ż����� �����尡 ������ �۾��� �����Ѵ�. 
	SetEvent(hEvent);//manual���� Event�� ������⿡, SetEvent�� signal�� TRUE�� ����. (�Ӱ迵��..?) 
	
	WaitForSingleObject(hThread1, INFINITE);//��ĥ�� ���� wait 
	WaitForSingleObject(hThread2, INFINITE);
	ResetEvent(hEvent);//signal�� FALSE�� ���� 
	CloseHandle(hEvent);
	
	return 0;
}

unsigned WINAPI NumberOfA(void *arg){//argument���� A�� ���� �� ī��Ʈ �� ��� 
	int i, cnt=0;
	WaitForSingleObject(hEvent, INFINITE);
	for(i=0; str[i]!=0; i++)
		if(str[i]=='A')
			cnt++;
	printf("Num of A: %d\n", cnt);
	return 0;
}

unsigned WINAPI NumberOfOthers(void *arg){//argument���� A�� �ƴ� �� ī��Ʈ �� ��� 
	int i, cnt=0;
	WaitForSingleObject(hEvent, INFINITE);
	for(i=0; str[i]!=0; i++)
		if(str[i]!='A')
			cnt++;
	printf("Num of others: %d\n", cnt-1);
	return 0;
}
