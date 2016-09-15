#include <stdio.h>

int main(){
	unsigned short ans=0;
	char* buf[20];
	int a=0x221;
	int i=0;
	memset(buf, 0, sizeof(buf));
	printf("a\n");
	memcpy(buf+4, &a, sizeof(a));
	printf("b\n");
	while(i<20){
		printf("%x", buf[i]);
		i++;
	}
}

