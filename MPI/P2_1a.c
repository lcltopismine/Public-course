#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

const int MAX_STRING = 100;
int main(int argc, char * argv[]){

	char *message = malloc(MAX_STRING * sizeof(char));

	int my_rank, process_count;

	if (MPI_Init(&argc, &argv)){
		printf("Error MPI_Init\n");
		exit(-1);
	}

	if (MPI_Comm_size(MPI_COMM_WORLD, &process_count)){
		printf("Error MPI_Comm_Size\n");
		exit(-1);
	}

	if (MPI_Comm_rank(MPI_COMM_WORLD, &my_rank)){
		printf("Error MPI_Comm_rank\n");
		exit(-1);
	}

	MPI_Status status;
	if (my_rank != 0) {
			sprintf(message, "First-round-process %d of %d> Have a nice day!", my_rank, process_count);
			MPI_Send(message, strlen(message)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	} else {
		for (int i = 1; i < process_count; i++){
			MPI_Recv(message, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);

			printf("First round: %d receive a message from %d with tag %d: %s\n", my_rank, status.MPI_SOURCE, status.MPI_TAG, message);
			fflush(stdout);

		}
	}

	printf("Process %d of %d> DONE in the first round!\n", my_rank, process_count);

	if (my_rank != 0) {
		sprintf(message, "Second-round-process %d of %d> Have a nice day!", my_rank, process_count);
		MPI_Send(message, strlen(message)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	} else {
		for (int i = 1; i < process_count; i++){
			// Now the receiving has order, start from 1 to p_c
			// Change i back to any source, or there is a dead lock
			// Master thread 0, slave thread 1:n
			MPI_Recv(message, MAX_STRING, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

			printf("Second round: %d receive a message from %d: %s\n", my_rank, i, message);
			fflush(stdout);

		}
	}

	printf("Process %d of %d> DONE in the second round!\n", my_rank, process_count);
	fflush(stdout);
	MPI_Finalize();
	free(message);
	return 0;
}
