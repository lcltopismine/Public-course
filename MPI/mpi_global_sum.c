#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>


int main(int argc, char * argv[]){
	//1. Initialize a global array and assign value
	int my_rank, process_count;
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
	long * global_sum_array = malloc(process_count * sizeof(long));
	int partner_rank,tag=0;
	long global_sum;
	long rec_sum;
	if (my_rank==0){
		for (int i=0;i<process_count;i++){
			global_sum_array[i] = i;
			if (i>0){
			MPI_Send(&global_sum_array[i], 1, MPI_LONG, i, tag, MPI_COMM_WORLD);
				}
		}
		global_sum = global_sum_array[0];
	}
	else{
		MPI_Recv(&rec_sum, 1, MPI_LONG, 0, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		global_sum = rec_sum;
	}
	//printf("Initial value of thread %d is %ld\n",my_rank,global_sum);
	for (int stride = 1; stride < process_count; stride *= 2){
		if (my_rank % stride == 0) {
			// active process
			partner_rank = my_rank % (2 * stride) == 0 ? my_rank + stride : my_rank - stride;
			if (my_rank % (2 * stride) == 0){
				MPI_Recv(&rec_sum, 1, MPI_LONG, partner_rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				global_sum += rec_sum;
			} else {
				MPI_Send(&global_sum, 1, MPI_LONG, partner_rank, tag, MPI_COMM_WORLD);
			}
			tag++;
		}
		}

	if(my_rank == 0){
		printf("The global sum is:%ld\n",global_sum);
	}

	MPI_Finalize();
	return 0;

}
