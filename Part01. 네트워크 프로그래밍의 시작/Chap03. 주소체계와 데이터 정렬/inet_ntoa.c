#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
	struct sockaddr_in addr1, addr2;
	char *str_ptr;
	char str_arr[20];
	
	addr1.sin_addr.s_addr=htonl(0x1020304);//host to network(to Big Endian) long
	addr2.sin_addr.s_addr=htonl(0x1010101);
	
	str_ptr=inet_ntoa(addr1.sin_addr);//addr1's address to string
	strcpy(str_arr, str_ptr);//copy (because inet_ntoa returns char* that can be removed anytime)
	printf("Dotted_Decimal notaion1: %s\n", str_ptr);//1.2.3.4
	
	inet_ntoa(addr2.sin_addr);
	printf("Dotted-Decimal notation2: %s\n", str_ptr);//���Ŀ� �ٲ� 
	printf("Dotted-Decimal notation3: %s\n", str_arr);//�̹� �����صξ��⿡ ��밡��. ���簡 Ű ����Ʈ! ���� �����͸� ��ȯ�ϱ⿡. 
	//1.1.1.1, 1.2.3.4
	return 0;
}
