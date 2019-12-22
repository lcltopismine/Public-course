#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define N 10000000
#define MAX_ERR 1e-6

__global__
void vector_add(float *out, float *a, float *b, int n) {
	int i = blockDim.x*blockIdx.x+threadIdx.x;
    if (i<n)
        out[i] = a[i] + b[i];

}

int main(){
    float *a, *b, *out; 

    // Allocate memory
    a   = (float*)malloc(sizeof(float) * N);
    b   = (float*)malloc(sizeof(float) * N);
    out = (float*)malloc(sizeof(float) * N);

    // Initialize array
    for(int i = 0; i < N; i++){
        a[i] = 1.0f;
        b[i] = 2.0f;
    }

    // Main function
    int block_size=256;
    dim3 gridDim((N-1)/block_size+1,1,1);
    dim3 blockDim(block_size,1,1);
    //transfer memory
    float *d_A,*d_B,*d_out;
    int size = sizeof(float) * N;
    cudaMalloc((void **)&d_A,size);
    cudaMalloc((void **)&d_B,size);
    cudaMalloc((void **)&d_out,size);
    cudaMemcpy(d_A,a,size,cudaMemcpyHostToDevice);
    cudaMemcpy(d_B,b,size,cudaMemcpyHostToDevice);
    vector_add<<<gridDim,blockDim>>>(d_out, d_A, d_B, N);
    cudaMemcpy(out,d_out,size,cudaMemcpyDeviceToHost);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_out);

    // Verification
    for(int i = 0; i < N; i++){
        assert(fabs(out[i] - a[i] - b[i]) < MAX_ERR);
    }

    printf("out[0] = %f\n", out[0]);
    printf("PASSED\n");
}
