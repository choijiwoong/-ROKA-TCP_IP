#include <stdio.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
	unsigned short host_port=0x1234;
	unsigned short net_port;
	unsigned long host_addr=0x12345678;
	unsigned long net_addr;
	
	net_port=htons(host_port);//host to network short
	net_addr=htonl(host_addr);//host to network long
	
	printf("Host ordered port: %#x\n", host_port);//Little Endian
	printf("Network ordered port: %#x\n", net_port);//to Big Endian!
	printf("Host ordered address: %#lx \n", host_addr);//Little Endian
	printf("Network ordered address: %#lx\n", net_addr);//to Big Endian!
	
	return 0;
}
