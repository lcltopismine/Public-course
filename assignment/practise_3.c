#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

long process_count=0, times=0;
long global_sum=0;
int block_size = 0;
sem_t sem;
void * pthreadGlobal_sum(void * thread_id);

int main (int argc, char * argv[]){
	process_count = strtol(argv[1],NULL,10);
	times = strtol(argv[2],NULL,10);
	pthread_t * thread_handler = malloc(process_count * sizeof(pthread_t));
	block_size = times%process_count==0?times/process_count:times/process_count+1;
	sem_init(&sem,0,1);
	for(int i=0;i<process_count;i++){
		pthread_create(&thread_handler[i], NULL, pthreadGlobal_sum, (void *)i);
	}
	for (int i=0;i<process_count;i++){
		pthread_join(thread_handler[i],NULL);
	}
	sem_destroy(&sem);
	printf("The global sum is:%d\n",global_sum);
	free(thread_handler);
}

void * pthreadGlobal_sum(void * thread_id){
	int local_sum = 0;
	int my_rank = (int) thread_id;
	int start=my_rank*block_size;
	int end = (my_rank+1)*block_size>times? times: (my_rank+1)*block_size;
	for (int i=start;i<end;i++){
		local_sum += i;
	}
	sem_wait(&sem);
	global_sum += local_sum;
	sem_post(&sem);
}
