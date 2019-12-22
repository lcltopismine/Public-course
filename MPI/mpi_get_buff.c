#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "./my_rand.h"

//TODO: Only Need to check MPI_Status.
int main(int argc, char * argv[]){
	int my_rank, process_count;

	int *recv_buf;
	int recv_count;

	int * collected_array;
	if (MPI_Init(&argc, &argv)){
		printf("Initialization failed!");
	}
	if (MPI_Comm_rank(MPI_COMM_WORLD, &my_rank)){
		printf("Failed to retrieve rank number!");
	}
	if (MPI_Comm_size(MPI_COMM_WORLD, &process_count)){
		printf("Failed to retrieve total process count!");
	}
	if (my_rank==0){
	int i,k;
	unsigned seed = 1,x;
	int max_meas=10,min_meas=0;
	x = my_rand(&seed);
	for (i=0;i<my_rank;i++){
		x = my_rand(&x);
	}
	k = (int)((max_meas-min_meas)*my_drand(&x)+min_meas);
	printf("The k is: %d\n",k);
	collected_array = malloc(k*sizeof(int));
	for (i=0;i<k;i++){
		collected_array[i] = i+1;
	}
	MPI_Send(collected_array, k, MPI_INT, 1, 0, MPI_COMM_WORLD);
	}
	else{
		MPI_Status status;
		int count;
		MPI_Probe(0,0,MPI_COMM_WORLD,&status);
		MPI_Get_count(&status,MPI_INT,&count);
		printf("The required buffer is: %d\n",count);
		collected_array = malloc(count*sizeof(int));
		MPI_Recv(collected_array, count, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	}
	MPI_Finalize();
	return 0;
}
