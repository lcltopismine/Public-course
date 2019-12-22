/*

*/
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void hello();

int main(void) {
	int thread_count = 6;
#pragma omp parallel num_threads(thread_count)
	{
    hello();
	}
	return 0;
}

void hello(){
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();
    printf("Good job!\n");
    printf("The rank is:%d and the thread_count is:%d\n",my_rank,thread_count);
}
