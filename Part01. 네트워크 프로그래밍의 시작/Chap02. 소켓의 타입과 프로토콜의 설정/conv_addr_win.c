#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <winsock2.h>

int main(int argc, char *argv[]){
	char *strAddr="203.211.218.102:9190";
	
	char strAddrBuf[50];
	SOCKADDR_IN servAddr;//����ü 
	int size;
	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);//�⺻ �¾� 
	
	size=sizeof(servAddr);//�ּ�ũ��. strAddr�� string���� IP�� servAddr SOCKADDR���Ŀ� �����Ѵ�. 
	WSAStringToAddress(strAddr, AF_INET, NULL, (SOCKADDR*)&servAddr, &size);//strAddr�� ������ AF_INET �������� �йи���, provider����(NULL) ��ȯ�� ����� servAddr�� �����ϴµ� ���� ũ�� size 
		
	size=sizeof(strAddrBuf);
	WSAAdressToString((SOCKADDR*)&servAddr, sizeof(servAddr), NULL, strAddrBuf, &size);//SOCKADDR������ strAddrBuf�� ���ڿ� �������� �����Ѵ�. 
	
	printf("Second conv result: %s\n", strAddrBuf);
	WSACleanup();
	
	return 0;
}//���� �Լ��� ���� �������� ���Ŀ� ���� �����ϴٴ� ������ ������, �����쿡 �������� �ڵ�⿡ ������� �ʴ´�. 
