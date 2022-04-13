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
		unsigned long conv_addr=inet_addr(addr);//������ �ּҷ� ��ȯ 
		if(conv_addr==INADDR_NONE)
			printf("Error occured!\n");
		else
			printf("Network ordered integer addr: %#lx\n", conv_addr);//��ȯ ��� ��� 
	}//(�������� �Ҹ��) 
	
	{//inet_ntoa
		struct sockaddr_in addr;
		char *strPtr;
		char strArr[20];
		
		addr.sin_addr.s_addr=htonl(0x1020304);//�������� ��Ʈ��ũ�ּҷ� ��ȯ 
		strPtr=inet_ntoa(addr.sin_addr);//��Ʈ��ũ �ּҸ� address�� ��ȯ(string) 
		strcpy(strArr, strPtr);//inet_ntoa�� char*�� �����ϱ⿡ ���� 
		printf("Dotted-Decimal notation3 %s\n", strArr);//��� ��� 
	} 
	
	WSACleanup();
	return 0;
}

void ErrorHandling(char* message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
