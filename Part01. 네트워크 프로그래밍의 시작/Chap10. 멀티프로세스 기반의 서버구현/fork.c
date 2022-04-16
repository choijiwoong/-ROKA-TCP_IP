#include <stdio.h>
#include <unistd.h>

int gval=10;
int main(int argc, char *argv[]){
	pid_t pid;
	int lval=20;
	gval++, lval+=5;
	
	pid=fork();//순간 완전히 동일한 다른 프로세스 생성. 
	if(pid==0)//if Child process. 자식은 0으로, 부모는 자식프로세스의 ID값을 갖음. 
		gval+=2, lval+=2;
	else//parent
		gval-=2, lval-=2;
		
	if(pid==0)
		printf("Child Proc: [%d, %d]\n", gval, lval);
	else
		printf("Parent Proc: [%d, %d]\n", gval, lval);
	
	return 0;
}
