#include <stdio.h>
#define BUF_SIZE 3//배열의 길이를 최소한으로 구성. 이유는 잘 모르겠는뎀 뭐 너무 커지면 커지는대로, 작으면 작은대로 별로인가보지. 욕심부리지말고 3배향상 
int main(int argc, char *argv[]){
	FILE * fp1;
	FILE * fp2;
	
	char buf[BUF_SIZE];
	
	fp1=fopen("news.txt", "r");
	fp2=fopen("cpy.txt", "w");
	
	while(fgets(buf, BUF_SIZE, fp1)!=NULL)
		fputs(buf, fp2);//표준 입출력 함수 사용으로서 성능향상만을 위한 전용 버퍼를 사용한다. 
		
	fclose(fp1);
	fclose(fp2);
	
	return 0;
}
