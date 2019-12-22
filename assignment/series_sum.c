#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	long N = strtol(argv[1], NULL,10);
	long i,sum;
	sum = 0;
	for(i=1;i<=N;i++){
		sum+=i;
	}
	printf("The sum is: %ld\n",sum);
}
