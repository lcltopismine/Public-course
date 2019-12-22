#include <stdio.h>
#include <mpi.h>

int main(){
	int my_rank;
	int comm_size;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	printf("Process %d of %d Have a good day!\n",my_rank,comm_size);
	MPI_Finalize();
	return 0;
}
