#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
	char *addr1="1.2.3.4";
	char *addr2="1.2.3.256";//유효하지 않은 IP 
	
	unsigned long conv_addr=inet_addr(addr1);//문자열 형식으로 표현된 IP주소를 32비트 정수형으로 변환해주며 네트워크 바이트 순서로의 변환을 진행해주는 함수. 
	if(conv_addr==INADDR_NONE)//실패시 반환 
		printf("Error occured!\n");
	else
		printf("Network ordered integer addr: %#lx\n", conv_addr);
		
	conv_addr=inet_addr(addr2);
	if(conv_addr==INADDR_NONE)//에러 검출! 
		printf("Error occureded\n");
	else
		printf("Network ordered integer addr: %#lx\n\n", conv_addr);
		
	return 0;
}
