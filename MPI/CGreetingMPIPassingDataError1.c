/**
 * Basic programming of MPI_Send and MPI_Recv
 *
 * The slave process may finish and terminate;
 * their sending message is possibly lost if the receiver did not respond timely
 *
 */

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
			sprintf(message, "First-round-process %d of %d> Have a nice day!\n", my_rank, process_count);
			//slave process, 0->receiver 0->tag MPI_COMM: sub domain
			MPI_Send(message, strlen(message)+1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
	} else {
		for (int i = 1; i < process_count; i++){
			// MAX_STRING is the size to reserve buffer for receive message.
			// This is incorrect.
			// MPI_ANY_SOURCE: from any source.
			MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

			printf("\nFirst round: %d receive a message from %d with tag %d: %s\n", my_rank, status.MPI_SOURCE, status.MPI_TAG, message);
			fflush(stdout);

		}
	}

	printf("Process %d of %d> DONE in the first round!\n", my_rank, process_count);

	fflush(stdout);
	MPI_Finalize();
	free(message);
	return 0;
}
