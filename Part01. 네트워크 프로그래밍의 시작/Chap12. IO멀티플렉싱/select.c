#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[]){
	fd_set reads, temps;//파일디스크립터를 담을 변수 
	int result, str_len;
	char buf[BUF_SIZE];
	struct timeval timeout;//select의 timeout에 전달할 값(블로킹 방지) 
	
	FD_ZERO(&reads);//fd_set을 초기화 
	FD_SET(0, &reads);//파일 디스크립터 0을 1로 세팅한다. 
	
	/* while반복할때마다 리셋되게 while안에서 실행. 
	timeout.tv_sec=5;
	timeout.tv_usec=3000;
	*/
	
	while(1){
		temps=reads;//현재 fd_set의 값을 복사 
		timeout.tv_sec=5;//timeout의 값을 세팅 
		timeout.tv_usec=0;
		result=select(1, &temps, 0, 0, &timeout);//1개의 파일 디스크립터(범위), fd_set(수신데이터존재여부에 관심있다면), writeset(블로킹없이전송여부), exceptset(예외여부), timeout전달 
		if(result==-1){//오류발생시 
			puts("select() error!");
			break;
		} else if(result==0){//timeout시(별다른 변화 없는채) 
			puts("Time-out!");
		} else{//변화가 발생시 
			if(FD_ISSET(0, &temps)){//0번 파일 디스크립터에 변화가 발생했는지 체크 후  (표준 입력이 맞는지) 아하! 파일디스크립터 0이 stdin 1이 stdout 2가 stderr니까!!! 
				str_len=read(0, buf, BUF_SIZE);//변화 발생시 버퍼의 값을 읽은 뒤 출력. 
				buf[str_len]=0;
				printf("message from console: %s", buf);
			}
		}
	}
	return 0;
}
