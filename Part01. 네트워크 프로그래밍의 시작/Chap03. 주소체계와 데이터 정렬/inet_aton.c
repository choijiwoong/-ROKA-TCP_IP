#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
void error_handling(char *message);

int main(int argc, char *argv[]){
	char *addr="127.232.124.79";
	struct sockaddr_in addr_inet;
	
	if(!inet_aton(addr, &addr_inet.sin_addr))//inet_addr은 변환만 하는 반면, inet_aton은 2nd인자로 sockaddr_int의 in_addr에 대입하는 과정까지도 진행해준다. 
		error_handling("Conversion error");
	else
		printf("Network ordered integer addr: %#x\n", addr_inet.sin_addr.s_addr);//sockaddr_in의 32비트 IP주소 보관을 in_addr에 하며 in_addr은 in_addr_t에 저장. 
	
	return 0;
} 

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
