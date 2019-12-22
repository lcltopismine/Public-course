#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <cuda.h>

#define N 10000000
#define MAX_ERR 1e-6


__global__
void vector_add_kernel(float *out, float *a, float *b, int n){
    int i = threadIdx.x + blockDim.x*blockIdx.x;
    //printf("%d,%d,%d\n",i,n,a[i]);
    if (i<n)
    	out[i] = a[i] + b[i];
    //printf("%d!",out[i]);
}

__host__
void vector_add(float *out, float *a, float *b, int n){
    int size = n * sizeof(float);
    float *d_A, *d_B, *d_out;
    cudaMalloc((void **) &d_A, size);
    cudaMemcpy(d_A, a, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_B, size);
    cudaMemcpy(d_B, b, size, cudaMemcpyHostToDevice);
    cudaMalloc((void **) &d_out, size);

    int block_size = 256;
    int total_block = (N + block_size -1)/block_size;
    //printf("%f,%f\n",a[0],b[0]);
    //printf("%d,%d", total_block,block_size);
    vector_add_kernel<<<total_block,block_size>>>(d_out, d_A, d_B, N);

    cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);
    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_out);
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
    vector_add(out, a, b, N);

    // Verification
    for(int i = 0; i < N; i++){
        assert(fabs(out[i] - a[i] - b[i]) < MAX_ERR);
    }

    printf("out[0] = %f\n", out[0]);
    printf("PASSED\n");
}
