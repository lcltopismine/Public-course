#include <cuda.h>
#include <stdio.h>

void hello();


int main(void) {
	cuda_hello<1,6>();
}

__global__
void cuda_hello()
{
	printf("Good job!\n");
}
