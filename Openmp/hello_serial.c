/* File:     hello_serial.c
 *
 * Purpose:  A serial hello, world program
 *
 * Compile:  gcc -g -Wall  -o hello_serial hello_serial.c
 * gcc -g -Wall -o name code.c
 * gcc -g -wall -fopenmp -o name code.c
 * Run:      ./hello_serial.c
 * 
 * Input:    none
 * Output:   A greeting message
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
void Hello(void);  /* Thread function */

/*--------------------------------------------------------------------*/
int main(int argc, char * argv[]) {
	if (argc < 1){
		printf("The usage: hello_serial $(num_threads)\n");
		exit(-1);
	}
	int num_threads = atoi(argv[1]);

	#pragma omp parallel num_threads(num_threads)
	{
    Hello();
	}
    return 0;
}  /* main */

/*-------------------------------------------------------------------
 * Function:    Hello
 * Purpose:     A function that prints message
 */
void Hello(void) {

	int thread_count = omp_get_num_threads();
	int my_rank = omp_get_thread_num();
	printf("Hello from %d of %d\n", my_rank, thread_count);

}  /* Hello */
