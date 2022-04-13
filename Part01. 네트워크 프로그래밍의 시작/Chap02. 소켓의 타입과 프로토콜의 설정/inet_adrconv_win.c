#include <stdio.h>
#include <string.h>
#include <winsock2.h>
void ErrorHandling(char* message);

int main(int argc, char *argv[]){
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2,2), &wsaData)!=0)
		ErrorHandling("WSAStartup() error!");
		
	{//inet_addr example
		char *addr="127.212.124.78";
		unsigned long conv_addr=inet_addr(addr);//정수형 주소로 변환 
		if(conv_addr==INADDR_NONE)
			printf("Error occured!\n");
		else
			printf("Network ordered integer addr: %#lx\n", conv_addr);//변환 결과 출력 
	}//(지역변수 소멸용) 
	
	{//inet_ntoa
		struct sockaddr_in addr;
		char *strPtr;
		char strArr[20];
		
		addr.sin_addr.s_addr=htonl(0x1020304);//정수형을 네트워크주소로 변환 
		strPtr=inet_ntoa(addr.sin_addr);//네트워크 주소를 address로 반환(string) 
		strcpy(strArr, strPtr);//inet_ntoa는 char*를 리턴하기에 복사 
		printf("Dotted-Decimal notation3 %s\n", strArr);//결과 출력 
	} 
	
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
