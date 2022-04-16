#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>//DNS
void error_handling(char *message);

int main(int argc, char *argv[]){
	int i;
	struct hostent *host;
	if(argc!=2){
		printf("Usage: %s <addr>\n", argv[0]);
		exit(1);
	}
	
	host=gethostbyname(argv[1]);//struct hostent 포인터 반환. h_name & h_aliases & h_addrtype & h_addr_list 
	if(!host)
		error_handling("gethost.... error");
	
	printf("Official name: %s\n", host->h_name);
	for(i=0; host->h_aliases[i]; i++)//마지막에 null값이 저장되어 있으니.. 
		printf("Aliases %d: %s\n", i+1, host->h_aliases[i]);
	printf("Address tpe: %s\n", (host->h_addrtype==AF_INET)?"AF_INET": "AF_INET6");
	for(i=0; host->h_addr_list[i]; i++)
		printf("IP addr %d: %s\n", i+1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));//실제 h_addr_list가 가리키는 것은 문자열 포인터가 아닌 in_addr이기에 캐스팅, string으로 변환 
		
	return 0;
}

void error_handling(char *message){
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
