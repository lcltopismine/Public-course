#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

const int MAX_INT_BUFFER = 1;
int main(int argc,char * argv[]){
	int my_rank, process_count, Partner;
	int count = 0;
	if (MPI_Init(&argc, &argv)){
		printf("Error MPI_init\n");
		exit(-1);
	}
	if (MPI_Comm_rank(MPI_COMM_WORLD,&my_rank)){
		printf("Improper rank\n");
		exit(-1);
	}
	if (MPI_Comm_size(MPI_COMM_WORLD,&process_count)){
		printf("Improper size\n");
		exit(-1);
	}
	MPI_Status status;
	Partner = (my_rank+1)%process_count;
	while (count<10){
		if (my_rank%2==0){
			count++;
			MPI_Send(&count, 1, MPI_INT, Partner, 0, MPI_COMM_WORLD);
			MPI_Recv(&count, MAX_INT_BUFFER, MPI_INT, Partner, 0, MPI_COMM_WORLD, &status);
			printf("received message from %d, with count of %d\n", status.MPI_SOURCE, count);
		}
		else{
			MPI_Recv(&count, MAX_INT_BUFFER, MPI_INT, Partner, 0, MPI_COMM_WORLD, &status);
			printf("received message from %d, with count of %d\n", status.MPI_SOURCE, count);
			count++;
			MPI_Send(&count, 1, MPI_INT, Partner, 0, MPI_COMM_WORLD);

		}
	}
	MPI_Finalize();
	return 0;
}
