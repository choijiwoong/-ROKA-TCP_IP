#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]){//<��� ���� ��ũ���Ͱ� �Ҹ�Ǿ�� ���ϵ� �Ҹ�ȴ�> 
	int cfd1, cfd2;
	char str1[]="Hi~\n";
	char str2[]="It's nice day~\n";
	
	//<������ ���� �Ǵ� ������ ������ ���� �� �ٸ� ���� ��ũ������ ����> 
	cfd1=dup(1);//���ϵ�ũ���� 1�� ����Ű�� ���� �Ǵ� ������ ����Ű�� �Ǵٸ� ���ϵ�ũ���� ��ȯ 
	cfd2=dup2(cfd1, 7);//��������� ������ ���ϵ�ũ���� ��ȣ ����(���μ����� ������ �� �ִ� ���� ��ũ������ ������ ���� ���� �Է��ؾ��ϴ°� ����. 0�̻��� �翬) 
	
	printf("fd1=%d, fd2=%d\n", cfd1, cfd2);//stdout
	write(cfd1, str1, sizeof(str1));//stdout����ǰ("Hi~\n")
	write(cfd2, str2, sizeof(str2));//stdout����ǰ2 ("It's nice day~\n")
	
	close(cfd1); close(cfd2);//���� �ݰ�**���⼭ �� �� �ִ� ���� dup, dup2�� �̿��� ����� ��ũ���ʹ� �ݾƵ� ���������� �۵��ϱ⿡ ������ ��ġ�� �ʴ´�.(fully-close�� ������� �ʴ´�) 
	write(1, str1, sizeof(str1));//ǥ�ؿ� ��½õ�("Hi~\n")
	close(1);//ǥ���� �ݰ� 
	write(1, str2, sizeof(str2));//ǥ�ؿ� ��½õ�(�ƹ��͵� ��µ��� ����) 
	
	return 0;
}
