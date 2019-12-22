/**
*	Matrix multiplication: C = A * B.
*	
*	This sample implements matrix multiplication which makes use of shared memory
*	to ensure data reuse, the matrix multiplication is done using tiling approach.
*/

//	System includes 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h> 
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

/**
*	Matrix multiplication : C = A * B
*	wA is A's width and wB is B's width
*/

void MatrixMul(float *C, float *A, float *B, int hA, int wA, int wB) { 
	for (int row = 0; row < hA; row++){
		for (int col = 0; col < wB; col++){
			C[row * wA + col] = 0;
			for (int k = 0; k < wA; k++){
				C[row * wA + col] += A[row * wA + k] * B[k * wB + col];
			}
		}
	}
}

/**
*	Matrix multiplication (CUDA Kernel) on the device: C = A * B
*	wA is A's width and wB is B's width
*	CUDA Kernel 1: each thread work independently on one element in C
*/
__global__
void MatrixMulCuda1(float *d_C, float *d_A, float *d_B, int hA, int wA, int wB) { 
	const int tile_size = 512;
	__shared__ float ds_M[tile_size][tile_size];
	__shared__ float ds_N[tile_size][tile_size];
	int row = threadIdx.y + blockIdx.y*blockDim.y;
	int col = threadIdx.x + blockIdx.y*blockDim.y;
	int tx = threadIdx.x;
	int ty = threadIdx.y;
	double p_val = 0;
	// assume divisible
	int num_phase = hA/tile_size;
	if (row<hA and col<wB){
		for(int i=0;i<num_phase;i++){
			ds_M[ty][tx] = d_A[row*width+i*tile_size+tx];
			ds_N[ty][tx] = d_B[(i*tile_size+ty)*width+col];
			__syncthreads();
			for(k=0;k<tile_size;k++){
				p_val += ds_M[ty][k] * ds_N[k][tx];
			}
			__syncthreads();
		}
		d_C[row*wA+col] = p_val;
	}
	
}

/**
*	Initialize the array
*/
void ConstantInit(float *data, int size, float val) {
	for (int i = 0; i < size; ++i) {
		data[i] = val;
	}
}

/**
*	Evaluate whether the CUDA method is successfully executed
*/
void checkCudaErrors(cudaError_t err){
	if (err != cudaSuccess)
	{
		fprintf(stderr, "Failed to allocate device vector B (error code %s)!\n", cudaGetErrorString(err));
		exit(EXIT_FAILURE);
	}
}

/**
*	Evaluate whether the CUDA method is successfully executed
*/
void evaluatePerformance(double msecTotal, int nIter, double nFloatOps, int threadsIdx_x, int threadsIdx_y){
	// Compute and print the performance
	float msecPerMatrixMul = msecTotal / nIter;
	double flopsPerMatrixMul = nFloatOps;
	double gigaFlops = (flopsPerMatrixMul * 1.0e-9f) / (msecPerMatrixMul / 1000.0f);
	printf(
		"Performance= %.2f GFlop/s, Time= %.3f msec, Size= %.0f Ops," \
		"	WorkgroupSize= %u threads/block\n", gigaFlops,
		msecPerMatrixMul,
		flopsPerMatrixMul, threadsIdx_x * threadsIdx_y);
}

/**
*	Run a simple test of matrix multiplication
*/
int MatrixMultiply(int hA, int wA, int wB) {
	//	Allocate host memory for matrices A and B 
	unsigned int size_A = hA * wA;
	unsigned int mem_size_A = sizeof(float) * size_A; 
	float *h_A = (float *)(malloc(mem_size_A));
	unsigned int size_B = wA * wB;
	unsigned int mem_size_B = sizeof(float) * size_B; 
	float *h_B = (float *)(malloc(mem_size_B));

	//	Initialize host memory 
	const float valB = 0.01f; 
	ConstantInit(h_A, size_A, 1.0f); 
	ConstantInit(h_B, size_B, valB);

	//	Allocate host matrix C
	unsigned int mem_size_C = hA * wB * sizeof(float); 
	float *h_C = (float *)(malloc(mem_size_C));
	if (h_C == NULL) {
		fprintf(stderr, "Failed to allocate matrix C!\n");
		exit(-1);
	}

	//	Allocate device memory 
	float *d_A, *d_B, *d_C;
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_A), mem_size_A));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_B), mem_size_B));
	checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_C), mem_size_C));

	// copy host memory to device
	checkCudaErrors(cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice));

	//	Allocate CUDA events that we'll use for timing 
	cudaEvent_t start; 
	checkCudaErrors(cudaEventCreate(&start));

	cudaEvent_t stop;
	checkCudaErrors(cudaEventCreate(&stop));

	//	Record the start event 
	checkCudaErrors(cudaEventRecord(start, NULL));

	//	Setup execution parameters
	int block_size = 4;
	dim3 threads(block_size, block_size);
	dim3 grid((wB + block_size - 1)/ block_size, (hA + block_size - 1) / block_size); 
	MatrixMulCuda1<<<grid, threads>>>(d_C, d_A, d_B, hA, wA, wB);

	/*
	struct timeval begin, end;
	gettimeofday(&begin, NULL);
	MatrixMulCuda1<<<grid, threads>>>(d_C, d_A, d_B, hA, wA, wB); 
	cudaDeviceSynchronize();
	gettimeofday(&end, NULL);
	long seconds = end.tv_sec - begin.tv_sec;
	long micros = (seconds * 1000000) + end.tv_usec - begin.tv_usec; 
	*/

	//	Record the stop event 
	checkCudaErrors(cudaEventRecord(stop, NULL));

	//	Wait for the stop event to complete 
	checkCudaErrors(cudaEventSynchronize(stop));

	float msecTotal = 0.0f;
	checkCudaErrors(cudaEventElapsedTime(&msecTotal, start, stop));
	evaluatePerformance(msecTotal, 1, (double)(2 * hA * wA * wB), block_size, block_size);

	// Copy result from device to host
	checkCudaErrors(cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost));

	printf("done\n");
	printf("Checking computed result for correctness: "); bool correct = true;

	//	test relative error by the formula
	//	|<x, y>_cpu - <x,y>_gpu|/<|x|, |y|> < eps double eps = 1.e-6; // machine zero

	for (int i = 0; i < (int)(hA * wB); i++) {
		double abs_err = abs(h_C[i] - (wA * valB));
		double dot_length = wA;
		double abs_val = abs(h_C[i]);
		double rel_err = abs_err / abs_val / dot_length;
		if (rel_err > eps) {
			printf("Error! Matrix[%05d]=%.8f, ref=%.8f error term is > %E\n", i, h_C[i], wA * valB, eps);
			correct = false;
		}
	}

	printf("%s\n", correct ? "Result = PASS" : "Result = FAIL");

	//	Clean up memory 
	free(h_A); 
	free(h_B); 
	free(h_C); 
	cudaFree(d_A); 
	cudaFree(d_B); 
	cudaFree(d_C);

	if (correct) {
		return 0;
	} else { 
		return -1;
	}
}

/**
*	Program main
*/
int main(int argc, char **argv) {
	printf("[Matrix Multiply Using C] - Starting...\n");

	int hA = 100;
	int wA = 100; 
	int wB = 100;
	printf(" -wA=WidthA:%d -hA=HeightA:%d (Width x Height of Matrix A)\n", wA, hA); 
	printf(" -wB=WidthB:%d -hB=HeightB:%d (Width x Height of Matrix B)\n", wB, wA);

	MatrixMultiply(hA, wA, wB);
}
