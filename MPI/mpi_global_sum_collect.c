#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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
	int tag=0;
	int global_sum;
	int local_sum=my_rank+1;
	MPI_Reduce(&local_sum,&global_sum,1,MPI_INT,MPI_SUM,tag,MPI_COMM_WORLD);
	if(my_rank == 0){
		printf("The global sum is:%d\n",global_sum);
	}

	MPI_Finalize();
	return 0;

}
