/*
 * testDataRace.c
 *
 *  Created on: Sep 17, 2019
 *      Author: qcheng1
 */

# include <stdio.h>
# include <pthread.h>
# include <stdlib.h>
pthread_mutex_t mut;

typedef struct{
	int thread_id;
	int *val;
} THData;


void * f ( void * p ) {
  /*
   * if we use &val in main
   * 1. We will not be able to get value of val, as this is passing address of val, not val itself
   * if we use long a = (long) p;
   * 2. if we use statement int * val = ( int *) p ,
   * then there is a race condition. Every thread tries to update pointer p.
   */
	THData *data = (THData *)p;
	int * val = data->val;
	//printf("The total is: %d\n",data->val);
	for ( int i =0; i < 100; ++ i )
		*val += 1;
	printf("The rank of current thread is: %d and the total is: %d\n",data->thread_id,data->val);
	return NULL ;
}

int main () {
	int con = 20;
	pthread_t th [con];
	int val = 0;
	//static array
	//THData thData[] = malloc(50 * sizeof(THData));
	//dynamic array
	//pass val of address, result no affect.
	THData* thData = malloc(con * sizeof(THData));
	for ( int i =0; i < con; ++ i ){
		thData[i].thread_id = i;
		thData[i].val = &val;
		pthread_create (& th [ i ] , NULL , f , &thData[i] );
	}

	for ( int i =0; i < con; ++ i )
		pthread_join ( th [ i ] , NULL );

	// this usually does not print 5 000 000
	printf("%d\n", val);
	return 0;
}
