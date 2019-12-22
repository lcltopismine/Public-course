

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

void ConstantInit(float *data, int size, float val) {
    for (int i = 0; i < size; ++i) {
        data[i] = val;
    }
}

/**
 * Run a simple test of matrix multiplication
 */
int MatrixMultiply(int hA, int wA, int wB) {

    // Allocate host memory for matrices A and B
    unsigned int size_A = hA * wA;
    unsigned int mem_size_A = sizeof(float) * size_A;
    float *A = (float *)(malloc(mem_size_A));
    unsigned int size_B = wA * wB;
    unsigned int mem_size_B = sizeof(float) * size_B;
    float *B = (float *)(malloc(mem_size_B));

    // Initialize host memory
    const float valB = 0.01f;
    ConstantInit(A, size_A, 1.0f);
    ConstantInit(B, size_B, valB);

    // Allocate host matrix C
    unsigned int mem_size_C = hA * wB * sizeof(float);
    float *C = (float *)(malloc(mem_size_C));

    if (C == NULL) {
        fprintf(stderr, "Failed to allocate matrix C!\n");
        exit(-1);
    }

	struct timeval begin, end;
	gettimeofday(&begin, NULL);

    MatrixMul(C, A, B, hA, wA, wB);

	gettimeofday(&end, NULL);

	long seconds = end.tv_sec - begin.tv_sec;
	long micros = (seconds * 1000000) + end.tv_usec - begin.tv_usec;

	printf("Time elapsed is %ld seconds and %ld micros\n", seconds, micros);

    printf("done\n");

    printf("Checking computed result for correctness: ");
    bool correct = true;

    // test relative error by the formula
    //     |<x, y>_cpu - <x,y>_gpu|/<|x|, |y|>  < eps
    double eps = 1.e-6;  // machine zero

    for (int i = 0; i < (int)(hA * wB); i++) {
        double abs_err = abs(C[i] - (wA * valB));
        double dot_length = wA;
        double abs_val = abs(C[i]);
        double rel_err = abs_err / abs_val / dot_length;

        if (rel_err > eps) {
            printf("Error! Matrix[%05d]=%.8f, ref=%.8f error term is > %E\n",
                   i, C[i], wA * valB, eps);
            correct = false;
        }
    }

    printf("%s\n", correct ? "Result = PASS" : "Result = FAIL");

    // Clean up memory
    free(A);
    free(B);
    free(C);

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

    if (argc < 4){
        printf("Usage -device=n (n >= 0 for deviceID)\n");
        printf("      -wA=WidthA -hA=HeightA (Width x Height of Matrix A)\n");
        printf("      -wB=WidthB -hB=HeightB (Width x Height of Matrix B)\n");
        printf("  Note: Outer matrix dimensions of A & B matrices" \
               " must be equal.\n");

        exit(EXIT_SUCCESS);
    }

    int hA = atoi(argv[1]);
    int wA = atoi(argv[2]);
    int wB = atoi(argv[3]);
    printf("MatrixA(%d,%d), MatrixB(%d,%d)\n", hA, wA,
                                               wA, wB);

    int matrix_result = MatrixMultiply(hA, wA, wB);

    exit(matrix_result);
}

