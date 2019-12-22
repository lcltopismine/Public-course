/* File:    trap.c
 * Purpose: Calculate definite integral using trapezoidal 
 *          rule.
 *
 * Input:   a, b, n
 * Output:  Estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: gcc -g -Wall -o trap trap.c
 * Usage:   ./trap
 *
 * Note:    The function f(x) is hardwired.
 *
 * IPP:     Section 3.2.1 (pp. 94 and ff.) and 5.2 (p. 216)
 */

#include <stdio.h>
#include <omp.h>
// 6 is good, 7 is not, different result
// we should expect exactly same result, but that's
// not the case. The reason is, openmp cannot properly
// handle the edge case well. And it may not be able to
// launch as the same number of thread as expected.
double f(double x);    /* Function we're integrating */
double Trap(double a, double b, int n, double h);

int main(int argc, char * argv[]) {
   double  approx=0;   /* Store result in integral   */
   double  a, b;       /* Left and right endpoints   */
   int     n;          /* Number of trapezoids       */
   double  h;          /* Height of trapezoids       */
   int thread_count;
   printf("Enter a, b, n and thread_count\n");
   scanf("%lf", &a);
   scanf("%lf", &b);
   scanf("%d", &n);
   scanf("%d", &thread_count);


   h = (b-a)/n;
   approx = h*(f(a)+f(b))/2.0;
   // parallel this part, what's local a,b,n?
   int i;
	#pragma omp parallel for num_threads(thread_count) \
		reduction(+: approx) private(i)
	  for(i=1;i<n;i++){
		  approx += h*f(a+i*h);
	  }
	  //approx += h * approx;

   printf("With n = %d trapezoids, our estimate ", n);
   printf("of the integral from %.2f to %.2f = %.2f\n",
      a, b, approx);

   return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Estimate integral from a to b of f using trap rule and
 *              n trapezoids
 * Input args:  a, b, n, h
 * Return val:  Estimate of the integral 
 */
double Trap(double a, double b, int n, double h) {
	int thread_count = omp_get_num_threads();
	int my_rank = omp_get_thread_num();
	int start, end, step;
	double integral;
	step = n/thread_count;
	start = a+h*my_rank * step;
	end = start+h*step;
	integral = (f(start) + f(end))/2.0;
   int k;
   for (k = 1; k < step; k++) {
     integral += f(start+k*h);
   }
   // global sum of integral
   integral = integral*h;
   printf("Gather result for thread id:%d result:%.2f\n",my_rank,integral);
   return integral;
}  /* Trap */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 */
double f(double x) {
   double return_val;

   return_val = x*x;
   return return_val;
}  /* f */
