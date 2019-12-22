

/**
 * Matrix multiplication: C = A * B.
 *
 * This sample implements matrix multiplication which makes use of shared memory
 * to ensure data reuse, the matrix multiplication is done using tiling approach.
 */

// System includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <sys/time.h>
#include <unistd.h>
#define TILESIZE 32
/**
 * Matrix multiplication : C = A * B
 * wA is A's width and wB is B's width
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
 * Matrix multiplication (CUDA Kernel) on the device: C = A * B
 * wA is A's width and wB is B's width
 * CUDA Kernel 1: each thread work independently on one element in C
 */
__global__
//<template int TILESIZE>
void MatrixMulCuda2(float *d_C, float *d_A, float *d_B, int hA, int wA, int wB, int tile_size) {
	//TODO
	// tile_size = block_size
	//const int TILE_SIZE = tile_size;
	int height = hA, width = wB;
//	const int TILE_SIZE = tile_size;
//	printf("The tile size is: %d",tile_size);
//	__shared__  float d_AS [32][32];
//	__shared__  float d_BS [32][32];
	__shared__  float d_AS [TILESIZE][TILESIZE];
	__shared__  float d_BS [TILESIZE][TILESIZE];
	int tx,ty;
	ty = threadIdx.y;
	tx = threadIdx.x;
	int col = tx + blockDim.x*blockIdx.x;
	int row = ty + blockDim.y*blockIdx.y;
	int phase_size = (((hA>wA?(hA>wB?hA:wB):(wA>wB?wA:wB))) + tile_size -1)/tile_size;
	//printf("%d,%d,%d,%d\n",hA,wA,wB,phase_size);
	float p_val = 0;
	if (row<=wB && col<=hA){
		for (int ph=0;ph<phase_size;++ph){

		d_AS[ty][tx] = d_A[row*wA+ph*tile_size+tx];
		d_BS[ty][tx] = d_B[(ph*tile_size+ty)*wB+col];
		__syncthreads();
		for (int k=0;k<tile_size;++k){
			p_val += d_AS[ty][k]*d_BS[k][tx];
		}
		__syncthreads();

//		p_val = 0;
		//__syncthreads();
		}
		d_C[row*wB+col] = p_val;
	}

}

__global__ void MatrixMulCuda1(float *d_C, float *d_A, float *d_B, int hA, int wA, int wB, int tile_size) {

	__shared__  float ds_M [TILESIZE][TILESIZE];
	__shared__  float ds_N [TILESIZE][TILESIZE];
	int bx = blockIdx.x;
	int by = blockIdx.y;
	int tx = threadIdx.x;
	int ty = threadIdx.y;
	int Row = by*blockDim.y+ty;
	int Col = bx*blockDim.x+tx;
	int phase_size = (((hA>wA?(hA>wB?hA:wB):(wA>wB?wA:wB))) + tile_size -1)/tile_size;
	float Pvalue = 0;
	for (int p=0;p<phase_size;++p){
		ds_M[ty][tx] = d_A[Row*wA + p*tile_size+tx];
		ds_N[ty][tx] = d_B[(p*tile_size+ty)*wA+Col];

	__syncthreads();
	for (int k=0;k<tile_size;++k){
		Pvalue += ds_M[ty][k]*ds_N[k][tx];
	}
	__syncthreads();
	d_C[Row*wB+Col] = Pvalue;
	}


}
/**
 * Initialize the array
 */
void ConstantInit(float *data, int size, float val) {
    for (int i = 0; i < size; ++i) {
        data[i] = val;
    }
}

/**
 * Evaluate whether the CUDA method is successfully executed
 */
void checkCudaErrors(cudaError_t  err){
	if (err != cudaSuccess)
    {
        fprintf(stderr, "Failed to allocate device vector B (error code %s)!\n", cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}

/**
 * Evaluate whether the CUDA method is successfully executed
 */
void evaluatePerformance(double msecTotal, int nIter, double nFloatOps, int threadsIdx_x, int threadsIdx_y){
	// Compute and print the performance
	float msecPerMatrixMul = msecTotal / nIter;
	double flopsPerMatrixMul = nFloatOps;
	double gigaFlops = (flopsPerMatrixMul * 1.0e-9f) /
					   (msecPerMatrixMul / 1000.0f);
	printf(
		"Performance= %.2f GFlop/s, Time= %.3f msec, Size= %.0f Ops," \
		" WorkgroupSize= %u threads/block\n",
		gigaFlops,
		msecPerMatrixMul,
		flopsPerMatrixMul,
		threadsIdx_x * threadsIdx_y);
}

/**
 * Run a simple test of matrix multiplication
 */
int MatrixMultiply(int hA, int wA, int wB) {

    // Allocate host memory for matrices A and B
    unsigned int size_A = hA * wA;
    unsigned int mem_size_A = sizeof(float) * size_A;
    float *h_A = (float *)(malloc(mem_size_A));
    unsigned int size_B = wA * wB;
    unsigned int mem_size_B = sizeof(float) * size_B;
    float *h_B = (float *)(malloc(mem_size_B));
    int block_size = 2;
    int tile_size = 2;
    // Initialize host memory
    const float valB = 0.01f;
    ConstantInit(h_A, size_A, 1.0f);
    ConstantInit(h_B, size_B, valB);

    // Allocate host matrix C
    unsigned int mem_size_C = hA * wB * sizeof(float);
    float *h_C = (float *)(malloc(mem_size_C));

    if (h_C == NULL) {
        fprintf(stderr, "Failed to allocate matrix C!\n");
        exit(-1);
    }

    // Allocate device memory
    float *d_A, *d_B, *d_C;

    checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_A), mem_size_A));

    checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_B), mem_size_B));

    checkCudaErrors(cudaMalloc(reinterpret_cast<void **>(&d_C), mem_size_C));

    // copy host memory to device
    checkCudaErrors(cudaMemcpy(d_A, h_A, mem_size_A, cudaMemcpyHostToDevice));

    checkCudaErrors(cudaMemcpy(d_B, h_B, mem_size_B, cudaMemcpyHostToDevice));

    // Allocate CUDA events that we'll use for timing
    cudaEvent_t start;
    checkCudaErrors(cudaEventCreate(&start));

    cudaEvent_t stop;
    checkCudaErrors(cudaEventCreate(&stop));

    // Record the start event
    checkCudaErrors(cudaEventRecord(start, NULL));

    // Setup execution parameters

    dim3 threads(block_size, block_size);
    dim3 grid((wB + block_size - 1)/ block_size, (hA + block_size - 1) / block_size);
    MatrixMulCuda2<<<grid, threads>>>(d_C, d_A, d_B, hA, wA, wB, tile_size);
    cudaDeviceSynchronize();
    /*
	struct timeval begin, end;
	gettimeofday(&begin, NULL);

    MatrixMulCuda1<<<grid, threads>>>(d_C, d_A, d_B, hA, wA, wB);
	cudaDeviceSynchronize();
	gettimeofday(&end, NULL);

	long seconds = end.tv_sec - begin.tv_sec;
	long micros = (seconds * 1000000) + end.tv_usec - begin.tv_usec;
	*/

    // Record the stop event
    checkCudaErrors(cudaEventRecord(stop, NULL));

    // Wait for the stop event to complete
    checkCudaErrors(cudaEventSynchronize(stop));

    float msecTotal = 0.0f;
    checkCudaErrors(cudaEventElapsedTime(&msecTotal, start, stop));

    evaluatePerformance(msecTotal, 1, (double)(2 * hA * wA * wB), block_size, block_size);

    // Copy result from device to host
    checkCudaErrors(cudaMemcpy(h_C, d_C, mem_size_C, cudaMemcpyDeviceToHost));


    printf("done\n");

    printf("Checking computed result for correctness: \n");
    bool correct = true;

    // test relative error by the formula
    //     |<x, y>_cpu - <x,y>_gpu|/<|x|, |y|>  < eps
    double eps = 1.e-6;  // machine zero

    for (int i = 0; i < (int)(hA * wB); i++) {
        double abs_err = abs(h_C[i] - (wA * valB));
        double dot_length = wA;
        double abs_val = abs(h_C[i]);
        double rel_err = abs_err / abs_val / dot_length;

        if (rel_err > eps) {
            printf("Error! Matrix[%05d]=%.8f, ref=%.8f error term is > %E\n",
                   i, h_C[i], wA * valB, eps);
            correct = false;

        }
    }
//    for (int i = 0; i < (int)(hA * wB); i++) {
//    	printf("%d,%f\n",i,h_C[i]);
//    }
//    correct = false;
    printf("%s\n", correct ? "Result = PASS" : "Result = FAIL");

    // Clean up memory
    free(h_A);
    free(h_B);
    free(h_C);

    if (correct) {
        return 0;
    } else {
        return -1;
    }
}


/**
 * Program main
 */
int main(int argc, char **argv) {
    printf("[Matrix Multiply Using C] - Starting...\n");

//    if (argc < 4){
//        printf("Usage -device=n (n >= 0 for deviceID)\n");
//        printf("      -wA=WidthA -hA=HeightA (Width x Height of Matrix A)\n");
//        printf("      -wB=WidthB -hB=HeightB (Width x Height of Matrix B)\n");
//        printf("  Note: Outer matrix dimensions of A & B matrices" \
//               " must be equal.\n");
//
//        exit(EXIT_SUCCESS);
//    }
//
//    int hA = atoi(argv[1]);
//    int wA = atoi(argv[2]);
//    int wB = atoi(argv[3]);
//    printf("MatrixA(%d,%d), MatrixB(%d,%d)\n", hA, wA,

    //test case: tile_size = 2, input = 5
    int hA = 100;
    int wA = 100;
    int wB = 100;
//    int hA = 5;
//	int wA = 5;
//	int wB = 5;
    MatrixMultiply(hA, wA, wB);

//	printf("Time elapsed is %ld seconds and %ld micros\n", seconds, micros);

//    exit(matrix_result);
}

