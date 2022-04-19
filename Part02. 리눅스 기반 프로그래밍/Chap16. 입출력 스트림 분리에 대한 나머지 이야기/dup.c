#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){//<모든 파일 디스크립터가 소멸되어야 소켓도 소멸된다> 
	int cfd1, cfd2;
	char str1[]="Hi~\n";
	char str2[]="It's nice day~\n";
	
	//<동일한 파일 또는 소켓의 접근을 위한 또 다른 파일 디스크립터의 생성> 
	cfd1=dup(1);//파일디스크립터 1이 가리키는 파일 또는 소켓을 가리키는 또다른 파일디스크립터 반환 
	cfd2=dup2(cfd1, 7);//명시적으로 복사할 파일디스크립터 번호 지정(프로세스당 생성할 수 있는 파일 디스크립터의 수보다 작은 값을 입력해야하는거 주의. 0이상은 당연) 
	
	printf("fd1=%d, fd2=%d\n", cfd1, cfd2);//stdout
	write(cfd1, str1, sizeof(str1));//stdout복제품("Hi~\n")
	write(cfd2, str2, sizeof(str2));//stdout복제품2 ("It's nice day~\n")
	
	close(cfd1); close(cfd2);//둘을 닫고**여기서 알 수 있는 것은 dup, dup2를 이용해 복사된 디스크립터는 닫아도 독립적으로 작동하기에 영향을 끼치지 않는다.(fully-close로 연쇄되지 않는다) 
	write(1, str1, sizeof(str1));//표준에 출력시도("Hi~\n")
	close(1);//표준을 닫고 
	write(1, str2, sizeof(str2));//표준에 출력시도(아무것도 출력되지 않음) 
	
	return 0;
}
