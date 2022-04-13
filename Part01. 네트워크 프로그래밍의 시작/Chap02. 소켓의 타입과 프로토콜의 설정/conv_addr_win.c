#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <winsock2.h>

int main(int argc, char *argv[]){
	char *strAddr="203.211.218.102:9190";
	
	char strAddrBuf[50];
	SOCKADDR_IN servAddr;//구조체 
	int size;
	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);//기본 셋업 
	
	size=sizeof(servAddr);//주소크기. strAddr의 string형식 IP를 servAddr SOCKADDR형식에 저장한다. 
	WSAStringToAddress(strAddr, AF_INET, NULL, (SOCKADDR*)&servAddr, &size);//strAddr의 내용을 AF_INET 프로토콜 패밀리로, provider없이(NULL) 변환한 결과를 servAddr에 저장하는데 문자 크기 size 
		
	size=sizeof(strAddrBuf);
	WSAAdressToString((SOCKADDR*)&servAddr, sizeof(servAddr), NULL, strAddrBuf, &size);//SOCKADDR내용을 strAddrBuf에 문자열 형식으로 저장한다. 
	
	printf("Second conv result: %s\n", strAddrBuf);
	WSACleanup();
	
	return 0;
}//위의 함수는 여러 프로토콜 형식에 적용 가능하다는 장점이 있지만, 윈도우에 종속적인 코드기에 사용하지 않는다. 
