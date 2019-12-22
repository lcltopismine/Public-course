#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "./my_rand.h"

int main(int argc, char * argv[]){
	//send bin_array to local
	//create local hist
	int my_rank, process_count, tag=0, local_bin_count;
	if (MPI_Init(&argc, &argv)){
		printf("Initialization failed!");
		exit(-1);
	}
	if (MPI_Comm_rank(MPI_COMM_WORLD,&my_rank)){
		printf("Failed to retrieve rank!\n");
		exit(-1);
	}
	if (MPI_Comm_size(MPI_COMM_WORLD,&process_count)){
		printf("Failed to retrieve process count!\n");
		exit(-1);
	}
	int * global_histogram;
	int * bound;
	long global_array_min=10000000,global_array_max=-10000000;
	if (my_rank==0){
		long data_count = strtol(argv[1], NULL,10);
		long min_meas = strtol(argv[2], NULL,10);
		long max_meas = strtol(argv[3], NULL,10);
		long bin_count = strtol(argv[4], NULL,10);

		int i,j;
		unsigned seed = 1,x;
		int n = 3;
		double * data = malloc(data_count*sizeof(double));
		double range = (max_meas-min_meas)/(bin_count+0.0);

		printf("The detected range is: %f\n",range*1000);
		x = my_rand(&seed);
		for (i=0;i<n;i++){
			x = my_rand(&x);
		}
		for (i=0;i<data_count;i++){
			data[i] = (max_meas-min_meas)*my_drand(&x)+min_meas;
			data[i] *= 1000;
			data[i] = (int)data[i];
			printf("The generated data is :%f\n",data[i]);
		}
		global_histogram = malloc(bin_count * sizeof(int));
		for (int i=0; i<bin_count;i++){
			global_histogram[i] = 0;
		}
		bound = malloc((bin_count+1) * sizeof(int));
		bound[0] = 0;
		for ( i=1; i<= bin_count; i++){
			bound[i] = bound[i-1]+range*1000;
		}
		bound[bin_count] = max_meas*1000;
		for ( j=0; j<=bin_count; j++){
			printf("The current bound is: %d\n",bound[j]);
		}
		// cut off propotion
		long per_part_num = data_count/(process_count-1)+1;
		long start, end;
		printf("per_part_num:%ld\n",per_part_num);
		// Debug from here

	}
	else{
		// receive bound;
		MPI_Recv(&local_bin_count, 5, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&local_data_length, 5, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		bound = malloc((local_bin_count+1) * sizeof(int));
		local_data = malloc(local_data_length*sizeof(int));
		MPI_Recv(&bound, 100, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&local_data, 100, MPI_INT, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// create local hist;
		local_histogram = malloc(local_bin_count * sizeof(int));
		for (int i=0; i<local_bin_count;i++){
			local_histogram[i] = 0;
		}
		for (int i=0; i<local_data_length; i++){
			if (local_data[i]>local_array_max){
				local_array_max = local_data[i];
			}
			if (local_data[i]<local_array_max){
				local_array_min = local_data[i];
			}

			for (int j=0; j<local_bin_count;j++){
				if ((local_data[i]>=bound[j]) && (local_data[i]<bound[j+1])){
					local_histogram[j]+=1;
				}
			}

		}

		// send local hist by order;
		MPI_Send(&local_array_max, 3, MPI_INT, 0, tag, MPI_COMM_WORLD);
		MPI_Send(&local_array_min, 3, MPI_INT, 0, tag, MPI_COMM_WORLD);
		MPI_Send(&local_histogram, local_bin_count, MPI_INT, 0, tag, MPI_COMM_WORLD);
	}
	//Retrieve global min/max
	MPI_reduce
	//Merge result
	if (my_rank==0){
		printf("The minimum of array is %ld\n",global_array_min);
		printf("The maximum of array is %ld\n",global_array_max);
		for (i=0;i<bin_count;i++){
			printf("histogram value for bin %d: %d\n",i,global_histogram[i]);
		}
	}
	MPI_Finalize();
	free(global_histogram);
	free(local_histogram);
	return 0;
}
