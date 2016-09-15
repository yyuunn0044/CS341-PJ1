#include <stdio.h>

int main(){

	char buf[1000];
	while(fgets(buf, 1000, stdin)){
		int i=0;
		while(i<1000){
			printf("%x ", buf[i]);
			i++;
		}
	}
}
