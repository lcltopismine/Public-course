#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include "./my_rand.h"
double * count_hist(long data_count, long bin_count, double * bound, double * data);
double * count_min_max(long data_count, double * data);
int main(int argc, char *argv[]) {
	long data_count = strtol(argv[1], NULL, 10);
	long min_meas = strtol(argv[2], NULL, 10);
	long max_meas = strtol(argv[3], NULL, 10);
	long bin_count = strtol(argv[4], NULL, 10);
	int thread_count = atoi(argv[5]);
	int i, j;
	unsigned seed = 1, x;
	double *data = malloc(data_count * sizeof(double));
	double range = (max_meas - min_meas) / (bin_count + 0.0);
	printf("The detected range is: %f\n", range);
	x = my_rand(&seed);

	double global_min = 10000000;
	double global_max = -10000000;
	//Generate elements for value
//#pragma omp parallel for num_threads(thread_count) private(i)
	for (i = 0; i < data_count; i++)
		data[i] = (max_meas - min_meas) * my_drand(&x) + min_meas;

//	for (i = 0; i < data_count; i++)
//	printf("The generated data is :%f\n", data[i]);

	printf("The total amount of data is: %ld\n", data_count);
	printf("The number of bin is: %ld\n", bin_count);
	// any other way to change to float?
	double *global_histogram = malloc(bin_count * sizeof(double));
	#pragma omp parallel for num_threads(thread_count)\
		private(i)
	for (i = 0; i < bin_count; i++)
		global_histogram[i] = 0;

	double *bound = malloc((bin_count + 1) * sizeof(double));
	bound[0] = 0;
	for (i = 1; i <= bin_count; i++) {
		bound[i] = bound[i - 1] + range;
	}
	bound[bin_count] = max_meas;
	for (j = 0; j <= bin_count; j++) {
		printf("The current bound is: %f\n", bound[j]);
	}
	// parallel this block
#pragma omp parallel num_threads(thread_count)
	{
		double * result = count_min_max(data_count, data);
		#pragma omp critical
		{
			if (global_min>result[0])
				global_min=result[0];
			if (global_max<result[1])
				global_max=result[1];
		}
	}
		printf("The max value of array is: %f\n", global_max);
		printf("The min value of array is: %f\n", global_min);
#pragma omp parallel num_threads(thread_count)
	{
	double * local_histogram = count_hist(data_count, bin_count, bound, data);
	#pragma omp critical
		{
			for (i = 0; i < bin_count; i++)
				global_histogram[i] += local_histogram[i];
		}
	}
	// end of parallel
	for (i = 0; i < bin_count; i++) {
		printf("histogram value for bin %d: %f\n", i, global_histogram[i]);
	}
}
double * count_min_max(long data_count, double * data){
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();
	int load_per_thread;
	load_per_thread = data_count%thread_count==0?\
		data_count/thread_count:data_count/thread_count+1;
	int start, end;
	start = my_rank * load_per_thread;
	end = (my_rank+1) * load_per_thread==data_count?data_count:(my_rank+1) * load_per_thread;
	double * result = malloc(2*sizeof(double));
	double min = 100000,max = -100000;
	for (int i=start;i<end;i++){
		if (data[i] > max) {
			max = data[i];
		}
		if (data[i] < min) {
			min = data[i];
		}
	}
	result[0] = min;
	result[1] = max;
	return result;
}


double * count_hist(long data_count, long bin_count, double * bound, double * data){
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();
	int load_per_thread;
	load_per_thread = data_count%thread_count==0?\
		data_count/thread_count:data_count/thread_count+1;
	int start, end, i ,j;
	start = my_rank * load_per_thread;
	end = (my_rank+1) * load_per_thread==data_count?data_count:(my_rank+1) * load_per_thread;
	double * local_histogram = malloc(bin_count * sizeof(double));
	for (i = 0; i < bin_count; i++)
		local_histogram[i] = 0;
	int k = 0;
	for (i = start; i < end; i++) {
		for (j = 0; j < bin_count + 1; j++) {
			k = j - 1;
			if (data[i] >= bound[j - 1] && data[i] < bound[j]) {
				local_histogram[k] += 1;
				break;
			}
		}
	}
	return local_histogram;
}
