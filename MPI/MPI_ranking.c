#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "./my_rand.h"

int TMPI_Rank(void *send_data, void *recv_data, MPI_Datatype datatype,
             MPI_Comm comm) {
// TODO: change type to double
	  int my_rank, process_count;
	  int * rank;
	  int * unorder;
	  MPI_Comm_size(comm, &process_count);
	  MPI_Comm_rank(comm, &my_rank);

	  int dsize;
	  MPI_Type_size(datatype, &dsize);
	  void * collect_array;
	  if (my_rank==0){
		  collect_array = malloc(dsize * process_count);
		  rank = malloc(dsize * process_count);
		  unorder = malloc(dsize * process_count);
		  for (int i = 0; i < process_count; i++) {
			  unorder[i] = collect_array[i];
		  }
	  }
	  MPI_Gather(send_data,1,datatype,collect_array,1,datatype,0,comm);
	  if (my_rank==0){
		  if (datatype == MPI_FLOAT) {
		    qsort(collect_array, process_count,
		          sizeof(collect_array), &compare_float_comm_rank_number);
		  } else {
		    qsort(collect_array, process_count,
		          sizeof(collect_array), &compare_int_comm_rank_number);
		  }
		  for (int i = 0; i < process_count; i++) {
			  for(int j=0;j< process_count;j++){
				  if (collect_array[i]==unorder[j])
				  rank[i] = j;
		  }
		  }

	  }
	  MPI_Scatter(rank, 1, MPI_INT, recv_data, 1, MPI_INT, 0, comm);
}
int main(int argc, char * argv[]){
	int my_rank, process_count;

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
	int rank;
	TMPI_Rank(&number, &rank, MPI_INT,
			MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;

}
