#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long thread_count = 0,times=0;
int block_size = 0;
int global_sum = 0,count = 0;
void * computeGlobalSum_pthread(void * Thread_ID);

int main(int * argc, char * argv[]){
	thread_count = strtol(argv[1],NULL,10);
	times = strtol(argv[2],NULL,10);
	block_size = times%thread_count==0 ? times/thread_count:times/thread_count+1;
	printf("The block_size is:%d\n",block_size);
	pthread_t* pthread_array = malloc(thread_count*sizeof(pthread_t));
	for (int i=0;i<thread_count;i++){
		pthread_create(&pthread_array[i], NULL, computeGlobalSum_pthread, (void*)i);
	}
	for (int i=0;i<thread_count;i++){
		pthread_join(pthread_array[i],NULL);
	}
	printf("The global_sum is:%d\n",global_sum);
	free(pthread_array);
}

void * computeGlobalSum_pthread(void * Thread_ID){
	int my_rank = (int) Thread_ID;
	int start = my_rank * block_size;
	int end = (my_rank+1) * block_size > times? times : (my_rank+1) * block_size;
	int local_sum = 0;
	for (int i=start;i<end;i++){
		local_sum+=i;
	}
	printf("For thread %d the local_sum is:%d\n",my_rank, local_sum);
	while (my_rank!=count);
	global_sum+=local_sum;
	count += 1;
	//busy-wait

}
