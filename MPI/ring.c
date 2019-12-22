#include <stdio.h>
#include <stdlib.h>
#include "my_rand.h"
#include <mpi.h>

int main(int argc, char * argv[]){
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
	MPI_Status status;
	unsigned seed = my_rank,x;
	int next_rank = (my_rank+1)%process_count;
	int pre_rank = my_rank==0?process_count-1:my_rank-1;
	int sum,recv_num;
	x = my_rand(&seed);
	x = my_rand(&x);
	//Assume that we generate number within range of 0-5
	int random_num = my_drand(&x)*10;
	if (my_rank==0){
		// only send message for rank=0
		printf("Random generated number at process %d is %d\n",my_rank,random_num);
		MPI_Send(&random_num, 1, MPI_INT, next_rank, 0, MPI_COMM_WORLD);
		MPI_Recv(&recv_num,1,MPI_INT,pre_rank,0,MPI_COMM_WORLD,&status);
		printf("Terminated at rank %d and The total sum is: %d\n",my_rank,recv_num);

	}
	else{
		MPI_Recv(&recv_num,1,MPI_INT,pre_rank,0,MPI_COMM_WORLD,&status);
		sum = random_num+recv_num;
		if (my_rank!=process_count){
			printf("Receive %d and send %d from process %d to current_rank %d with value %d\n", recv_num, random_num,status.MPI_SOURCE, my_rank, sum);
			MPI_Send(&sum, 1, MPI_INT,next_rank,0,MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
}
