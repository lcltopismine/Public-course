#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "./my_rand.h"
//mutex lock/unlock
long sum = 0;
long data_count,bin_count,block_size = 0;
double global_min = 10000000, global_max=-10000000;
int thread_count = 0;
int flag = 0;
double *data,*bound;
int *global_histogram;
pthread_mutex_t min_max_mutex;

typedef struct{
	int thread_id;
	double array_min;
	double array_max;
} array_data;


void* computing_min_max_pthreads(void * threadID);

int main(int argc, char *argv[]){
	data_count = strtol(argv[1], NULL,10);
	long min_meas = strtol(argv[2], NULL,10);
	long max_meas = strtol(argv[3], NULL,10);
	bin_count = strtol(argv[4], NULL,10);
	long thread_count = strtol(argv[5], NULL,10);
	int i,j;
	unsigned seed = 1,x;
	int n = 3;
	data = malloc(data_count*sizeof(double));
	double range = (max_meas-min_meas)/(bin_count+0.0);
	block_size = data_count % thread_count == 0 ? data_count/thread_count : data_count/thread_count + 1;
	pthread_t * thread_handlers = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	array_data * data_handler = (array_data*)malloc(thread_count * sizeof(array_data));
	int est_min = 10000000,est_max=-100000000;
	printf("The detected range is: %f\n",range);
	x = my_rand(&seed);
	for (i=0;i<n;i++){
		x = my_rand(&x);
	}
	for (i=0;i<data_count;i++){
		data[i] = (max_meas-min_meas)*my_drand(&x)+min_meas;
		printf("The generated data is :%f\n",data[i]);
	}
	global_histogram = malloc(bin_count * sizeof(int));
	for (int i=0; i<bin_count;i++){
		global_histogram[i] = 0;
	}
	bound = malloc((bin_count+1) * sizeof(double));
	bound[0] = 0;
	for ( i=1; i<= bin_count; i++){
		bound[i] = bound[i-1]+range;
	}
	bound[bin_count] = max_meas;
	for ( j=0; j<=bin_count; j++){
		printf("The current bound is: %f\n",bound[j]);
	}

	for (int thread = 0; thread < thread_count; thread++){
		data_handler[thread].array_max = est_max;
		data_handler[thread].array_min = est_min;
		data_handler[thread].thread_id = thread;
		if (pthread_create(&thread_handlers[thread], NULL, computing_min_max_pthreads, &data_handler[thread])){
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

	printf("The max value of array is: %f\n",global_max);
	printf("The min value of array is: %f\n",global_min);
	printf("The total amount of data is: %ld\n",data_count);
	printf("The number of bin is: %ld\n",bin_count);

	pthread_mutex_destroy(&min_max_mutex);
	//double data[16] = {1.3,2.9,0.4,0.3,1.3,4.4,1.7,0.3,3.2,0.3,4.9,2.4,3.1,4.4,3.9,0.4};
	// any other way to change to float?

	for (i=0;i<bin_count;i++){
		printf("histogram value for bin %d: %d\n",i,global_histogram[i]);
	}

	free(thread_handlers);
	free(data_handler);
	return EXIT_SUCCESS;
}

void* computing_min_max_pthreads(void * p){
	// Local operation
	array_data *ad = (array_data *) p;
	long my_rank = ad->thread_id;
	int * local_histogram = malloc(bin_count * sizeof(int));
	double min = ad->array_min;
	double max = ad->array_max;
	int k=0;
	long my_first_id = block_size * my_rank;
	long my_last_id = block_size * (my_rank + 1) > data_count ? data_count : block_size * (my_rank + 1);
	//printf("the data[0]:%f %f %ld\n",min,ad->array_min,my_rank);
	//printf("The block size is:%ld, 1st and end is: %ld, %ld\n",block_size,my_first_id,my_last_id);

	for (long i = my_first_id; i < my_last_id; i++){
		if (data[i]>max){
			max = data[i];
		}
		if (data[i]<min){
			min = data[i];
		}

		for(int j=1;j<bin_count+1;j++){
			k=j-1;
			if (data[i]>=bound[j-1] && data[i]<bound[j]){
				local_histogram[k]+=1;
				break;
			}
		}

	}
	// Cooperationg with shared memory, sum
	pthread_mutex_lock(&min_max_mutex);
	global_min = global_min>min? min:global_min;
	global_max = global_max<max? max:global_max;
	for (k=0;k<bin_count;k++){
		global_histogram[k]+=local_histogram[k];
	}
	pthread_mutex_unlock(&min_max_mutex);
	printf("For thread %ld the min is: %f the max is: %f\n",my_rank,min,max);
	return NULL;
}
