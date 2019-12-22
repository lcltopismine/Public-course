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
#include <time.h>

double * global_sum;
int count=0 , block_size = 0;
int thread_count = 0;
int flag = 0;
long depth;
sem_t * count_sems;
sem_t * barrier_sems;
long n = 100000;


// TODO:
// check log function
// check sem initialization
void* computingPiParallel(void * threadID);

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
	clock_t start = clock();
	thread_count = strtol(argv[1], NULL, 10);
	if (thread_count <= 0){
		printf("Usage: ./computingPi_pthreads  #(threads)\n");
		printf("#(threads) > 0\n");
		exit(1);
	}
	block_size = n % thread_count == 0 ? n/thread_count : n/thread_count + 1;

	depth = log(thread_count*1.0)/log(2);
	printf("The depth and n :%ld",depth);
	if (power(2, depth)<thread_count){
		depth+=2;
	}
	else{
		depth+=1;
	}
	printf("The depth and n :%ld",depth);
	pthread_t * thread_handlers = malloc(thread_count * sizeof(pthread_t));
	global_sum = malloc(thread_count * sizeof(pthread_t));
	count_sems = malloc(depth * sizeof(sem_t));
	barrier_sems = malloc(depth * sizeof(sem_t));
	// check sem initialization
	for (int o = 0; o<depth;o++){
		sem_init(&count_sems[o],0,1);
		sem_init(&barrier_sems[o],0,0);
	}
	for (int thread = 0; thread < thread_count; thread++){
		if (pthread_create(&thread_handlers[thread], NULL, computingPiParallel, (void *)thread)){
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

	for (int o =0; o<depth;o++){
		sem_destroy(&count_sems[o]);
		sem_destroy(&barrier_sems[o]);
	}

	free(thread_handlers);
	free(count_sems);
	free(barrier_sems);

	printf("Pi is equal to %f\n", global_sum[0]);
	free(global_sum);
    clock_t end = clock();
	return EXIT_SUCCESS;
}

void* computingPiParallel(void * threadID){
	double factor;
	long i;
	long my_rank = (long) threadID;
	double local_sum = 0;
	long my_first_id = block_size * my_rank;
	long my_last_id = block_size * (my_rank + 1) > n ? n : block_size * (my_rank + 1);

	for (i = my_first_id; i < my_last_id; i++){
		if (i % 2 == 0){
			factor = 1.0;
		} else {
			factor = -1.0;
		}
		//printf("before %ld",local_sum);
		//printf("%ld\n",i);
		local_sum += 4 * factor / (2*i+1);
		//printf("after %ld",local_sum);
	}

	//Here implements a barrier method
	//Cut to two stages. Compute each local sum and merge them.
	//printf("block_size, 1id, 2id, %ld %ld %ld\n",block_size,my_first_id,my_last_id);
	//printf("The local_sum is:%.16f \n",local_sum);
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
