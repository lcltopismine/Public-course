#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
int main(int argc, char *argv[]){
	long N = strtol(argv[1], NULL,10);
	int thread_count = atoi(argv[2]);
	long i,sum;
	sum = 0;
#pragma omp parallel for num_threads(thread_count)\
	reduction (+:sum) private(i)
	for(i=1;i<=N;i++){
		sum+=i;
	}
	printf("The sum is: %ld\n",sum);
}
