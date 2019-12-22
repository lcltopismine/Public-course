/* File:     hello_serial.c
 *
 * Purpose:  A serial hello, world program
 *
 * Compile:  gcc -g -Wall  -o hello_serial hello_serial.c
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
int main(void) {
	int thread_counts = 8;
#pragma omp parallel num_threads(thread_counts)
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

   printf("Hello !\n");
   int thread_count = omp_get_num_threads();
   int my_rank = omp_get_thread_num();

}  /* Hello */
