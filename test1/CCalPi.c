/*
 ============================================================================
 Name        : CTest.c
 Author      : Q. C.
 Version     :
 Copyright   : uncc.cci.cs
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double computingPiSerial();

int main(void) {
    clock_t start = clock();
    double sum = computingPiSerial();
    clock_t end = clock();
    double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;

	printf("Pi is equal to %f\n", sum);
	printf("The time consumed is %.4f\n", time_elapsed_in_seconds*1000000);

	return EXIT_SUCCESS;
}

double computingPiSerial(){
	double sum = 0;
	double factor;
	long i, n = 100000;

	for (i = 0; i < n; i++){
		if (i % 2 == 0){
			factor = 1.0;
		} else {
			factor = -1.0;
		}

		sum = sum + 4 * factor / (2*i+1);
	}
	return sum;
}
