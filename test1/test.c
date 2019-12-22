#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	long data_count = strtol(argv[1], NULL,10);
	long min_meas = strtol(argv[2], NULL,10);
	long max_meas = strtol(argv[3], NULL,10);
	long bin_count = strtol(argv[4], NULL,10);
	int i,j;
	unsigned seed = 1,x;
	int n = 3;
	double min=10000000;
	double max=-10000000;
	double *data = malloc(data_count*sizeof(double));
	double range = (max_meas-min_meas)/(bin_count+0.0);
	printf("The detected range is: %f\n",range);
	x = my_rand(&seed);
	for (i=0;i<n;i++){
		x = my_rand(&x);
	}
	for (i=0;i<data_count;i++){
		data[i] = (max_meas-min_meas)*my_drand(&x)+min_meas;
		if (data[i]>max){
			max = data[i];
		}
		if (data[i]<min){
			min = data[i];
		}
		printf("The generated data is :%f\n",data[i]);
	}
	printf("The max value of array is: %f\n",max);
	printf("The min value of array is: %f\n",min);
	printf("The total amount of data is: %ld\n",data_count);
	printf("The number of bin is: %ld\n",bin_count);
	//double data[16] = {1.3,2.9,0.4,0.3,1.3,4.4,1.7,0.3,3.2,0.3,4.9,2.4,3.1,4.4,3.9,0.4};
	// any other way to change to float?


	int *histogram = malloc(bin_count * sizeof(int));
	for (int i=0; i<bin_count;i++){
		histogram[i] = 0;
	}
	double *bound = malloc((bin_count+1) * sizeof(double));
	bound[0] = 0;
	for ( i=1; i<= bin_count; i++){
		bound[i] = bound[i-1]+range;
	}
	bound[bin_count] = max_meas;
	for ( j=0; j<=bin_count; j++){
		printf("The current bound is: %f\n",bound[j]);
	}
	int k=0;
	for (i=0;i<data_count;i++){
		for(j=0;j<bin_count+1;j++){
			k=j-1;
			if (data[i]>=bound[j-1] && data[i]<bound[j]){
				histogram[k]+=1;
				break;
			}
		}
	}
	for (i=0;i<bin_count;i++){
		printf("histogram value for bin %d: %d\n",i,histogram[i]);
	}

}
