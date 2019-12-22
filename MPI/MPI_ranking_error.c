#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "./my_rand.h"

int main(int argc, char * argv[]){
	int my_rank, process_count;
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
	int i;
	unsigned seed = 1,x;
	int max_meas=4000,min_meas=0,number;
	x = my_rand(&seed);
	for (i=0;i<my_rank;i++){
		x = my_rand(&x);
	}
	number = (int)((max_meas-min_meas)*my_drand(&x)+min_meas);
	printf("For process %d, val:%d\n",my_rank,number);
	if (my_rank==0){
		collected_array = malloc((process_count)*sizeof(int));
	}
	MPI_Gather(&number, 1, MPI_INT, collected_array, 1, MPI_INT, 0, MPI_COMM_WORLD);
	if (my_rank==0){
	for (int i=0;i<process_count;i++){
		printf("%d %d\n",i,collected_array[i]);
	}
	}
	MPI_Finalize();
	return 0;

}
