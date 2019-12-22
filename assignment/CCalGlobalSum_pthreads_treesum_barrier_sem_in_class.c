/*
 ============================================================================
 Name        : CTest.c
 Author      : Q. C.
 Version     :
 Copyright   : uncc.cci.cs
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

long * global_sum;
int * count;
long sum = 0;
long times = 0,block_size = 0;
int thread_count = 0;
int flag = 0;
long depth;

pthread_mutex_t * count_mutex;
int count = 0;



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
	int levels = ceil(log(thread_count)/log(2));
	count_mutex = malloc (levels * sizeof(pthread_mutex_t));
	for (int i=0;i<levels;i++){
		pthread_mutex_init(&count_mutex[i],NULL);
	}
	depth = log(thread_count*1.0)/log(2);
	printf("The depth and n :%ld",depth);
	if (power(2, depth)<thread_count){
		depth+=2;
	}
	else{
		depth+=1;
	}
	count = malloc(levels,sizeof(int));

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

//	1. What happened if we firstly barrier
//	2. What happened if set count!=thread_count-1
//  3. ilevel-1
	int iLevel = 0;
	for (int stride = 1;stride<thread_count;stride *=2){
		if (my_rank %(2*stride)==0){
			thread_sum[my_rank] += thread_sum[my_rank+stride];
			//barrier

			/*
			pthread_mutex_lock(&count_mutex[ilevel]);
			count[iLevel]+=1;
			pthread_mutex_unlock(&count_mutex[ilevel]);
			while (count!=thread_count/(2*stride));
			iLevel++;
			*/
			sem_t iSem = count_barrier[iLevel];
			sem_wait(&iSem);
			pthread_mutex_lock(&count_mutex[iLevel]);
			if (count[iLevel] == thread_count/(2*stride)-1){
				// release barrier
				count[iLevel] = 0;
				for (i=0;i<thread_count/(2*stride)-1;i++){
					sem_post(&iSem);
				}
			}
			else{

				count[iLevel]++;
				pthread_mutex_unlock(&count_mutex[iLevel]);
				sem_wait(&iSem);

			}
			iLevel++;
		}
	}

	return NULL;
}
