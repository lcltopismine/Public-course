#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <sys/time.h>
#include <unistd.h>

long * global_sum;
int count = 0;
long sum = 0;
long times = 0,block_size = 0;
int thread_count = 0;
int flag = 0;
long depth;
sem_t * count_sems;
sem_t * barrier_sems;


void* computingGlobalSum_pthreads(void * threadID);

int power(int base, int exp){
	//Refer from https://stackoverflow.com/
	//questions/4843304/why-is-my-power-operator-not-working
    int result = 1;
    while(exp) {
    	result *= base;
    	exp--;
    }
    return result;
}


int main(int argc, char * argv[]) {

	thread_count = strtol(argv[1], NULL, 10);
	times = strtol(argv[2], NULL, 10);

	if ((thread_count <= 0) || (times <= 0)){
		printf("Usage: ./computingPi_pthreads  #(threads) N_times\n");
		printf("#(threads) > 0 and N_times > 0\n");
		exit(1);
	}

	depth = log(thread_count*1.0)/log(2);
	printf("The depth and n :%ld",depth);
	if (power(2, depth)<thread_count){
		depth+=2;
	}
	else{
		depth+=1;
	}

	block_size = times % thread_count == 0 ? times/thread_count : times/thread_count + 1;
	// allocate a container of thread handlers
	pthread_t * thread_handlers = malloc(thread_count * sizeof(pthread_t));
	global_sum = malloc(thread_count * sizeof(pthread_t));
	count_sems = malloc(depth * sizeof(sem_t));
	barrier_sems = malloc(depth * sizeof(sem_t));
	for (int o =0; o<depth;o++){
		sem_init(&count_sems[o],0,1);
		sem_init(&barrier_sems[o],0,0);
	}
	struct timeval begin, end;
	gettimeofday(&begin, NULL);
	for (int thread = 0; thread < thread_count; thread++){
		if (pthread_create(&thread_handlers[thread], NULL, computingGlobalSum_pthreads, (void *)thread)){
			printf("Error creating a thread (id: %ld)\n", thread);
			exit(1);
		}
	}

	printf("%d threads are working on their local calculation and synchronizing with master thread\n", thread_count);

	for (int thread = 0; thread < thread_count; thread++){
		if (pthread_join(thread_handlers[thread], NULL)){
			printf("Error joining a thread (id: %ld)\n", thread);
			exit(1);
		}
	}
	gettimeofday(&end, NULL);
	long seconds = end.tv_sec - begin.tv_sec;
	long micros = (seconds * 1000000) + end.tv_usec - begin.tv_usec;
	printf("Thread_count=%d;N=%ld;Sum=%ld;Seconds=%ld;Micros=%ld\n", thread_count, times, sum, seconds, micros);
	// TODO:
	// free
	for (int o =0; o<depth;o++){
		sem_destroy(&count_sems[o]);
		sem_destroy(&barrier_sems[o]);
	}

	free(thread_handlers);
	free(count_sems);
	free(barrier_sems);
	printf("Global sum from 0 to %ld is equal to %ld\n", times-1, global_sum[0]);
	free(global_sum);
	return EXIT_SUCCESS;
}

void* computingGlobalSum_pthreads(void * threadID){
	// Local operation
	long my_rank = (long) threadID;

	long local_sum = 0;
	long my_first_id = block_size * my_rank;
	long my_last_id = block_size * (my_rank + 1) > times ? times : block_size * (my_rank + 1);

	for (long i = my_first_id; i < my_last_id; i++){
		local_sum += i;
	}
	printf("Local sum of thread %ld from %ld to %ld is equal to %ld\n", my_rank, my_first_id, my_last_id, local_sum);

	global_sum[my_rank] = local_sum;

	int s;
	sem_wait(&count_sems[0]);
	if (count==thread_count-1){
		count = 0;
		sem_post(&count_sems[0]);
		for (s=0; s<thread_count;s++){
			sem_post(&barrier_sems[0]);
		}
	}
	else{
		count++;
		printf("The count is:%d \n",count);
		sem_post(&count_sems[0]);
		sem_wait(&barrier_sems[0]);
	}

	for (s=1;s<depth;s++){
		if (my_rank % power(2,s)==0){
			//printf("%ld %ld %ld\n",s , my_rank,my_rank+power(2,s-1));
			if (my_rank+power(2,s-1)<thread_count){
				global_sum[my_rank] += global_sum[my_rank+power(2,s-1)];
			}
		}
		sem_wait(&count_sems[s]);
		if (count==thread_count-1){
			count = 0;
			sem_post(&count_sems[s]);
			for (int v=0; v<thread_count;v++){
				sem_post(&barrier_sems[s]);
			}
		}
		else{
			count++;
			sem_post(&count_sems[s]);
			sem_wait(&barrier_sems[s]);
		}

	}

	return NULL;
}
