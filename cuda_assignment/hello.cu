#include <stdio.h>

__global__
void hello(void){
	printf("Hello from the GPU\n");
}

int main(void){
	printf("Hello from the CPU\n");
	dim3 gridDim(2,3,1);
	// setup 6 blocks
	dim3 blockDim(10,2,3);
	// setup 6 blocks, a diff setup;
	hello<<<gridDim,blockDim>>>();
	cudaDeviceReset();
	return 0;
}
