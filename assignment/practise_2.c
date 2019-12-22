#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

long times=0,thread_count=0;
int block_size = 0, global_sum = 0;
pthread_mutex_t mutex;
void * globalSum_pthread(void * Thread_ID);

int main(int* argc,char * argv[]){
	thread_count = strtol(argv[1],NULL,10);
	times = strtol(argv[2],NULL,10);
	pthread_t * thread_handler = malloc(thread_count*sizeof(pthread_t));
	block_size = times%thread_count ==0? times/thread_count:times/thread_count+1;
	printf("block_size: %d\n",block_size);
	pthread_mutex_init(&mutex,NULL);
	for(int i=0;i<thread_count;i++){
		pthread_create(&thread_handler[i],NULL,globalSum_pthread,(void *)i);
	}
	for (int i=0;i<thread_count;i++){
		pthread_join(thread_handler[i],NULL);
	}
	printf("The global_sum is %d\n",global_sum);
	pthread_mutex_destroy(&mutex);
	free(thread_handler);
}

void * globalSum_pthread(void * Thread_ID){
	int local_sum=0;
	int my_rank = (int) Thread_ID;
	int start = block_size * my_rank;
	int end = block_size * (my_rank + 1) > times ? times : block_size * (my_rank + 1);
	for (int i=start;i<end;i++){
		local_sum+=i;
	}
	printf("echo start %d and end %d \n",start,end);
	printf("The local_sum is: %d\n",local_sum);
	pthread_mutex_lock(&mutex);
	global_sum+=local_sum;
	pthread_mutex_unlock(&mutex);
}
