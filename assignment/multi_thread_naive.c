#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *localsum(void* rank);
int block;
long global_sum;
long N,core;
int main(int argc, char *argv[]){
	/*
	 * there is no action taken for critical section
	 * As we take care of them in next set of questions
	 */
	N = strtol(argv[1], NULL,10);
	core = strtol(argv[2], NULL,10);
	long thread;
	block = N % core == 0 ? N/core:N/core+1;
	//printf("The N and block is:%ld %ld ",N,block);
	global_sum = 0;
	pthread_t* thread_handles;
	thread_handles = malloc(N*sizeof(pthread_t));
	for(thread=0;thread<core;thread++){
		pthread_create(&thread_handles[thread],NULL,localsum,
				(void*) thread);
	}

	for(thread=0;thread<core;thread++){
		pthread_join(thread_handles[thread],NULL);
	}
	free(thread_handles);
	printf("The sum is: %ld\n",global_sum);
	return 0;
}
void *localsum(void* rank){
	long my_rank = (long) rank;
	int start = block*my_rank+1;
	int end = block*(my_rank+1)<=N? block*(my_rank+1):N;
	long local_sum = 0;
	for (int i=start;i<=end;i++){
		local_sum+=i;
	}
	printf("For the thread %ld, the local sum is: %ld\n",my_rank,local_sum);
	global_sum+=local_sum;
	return NULL;
}
