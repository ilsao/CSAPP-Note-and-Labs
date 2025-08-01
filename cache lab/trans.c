/* 
 * Author: asciibase64
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    // Cache info: S = 32, E = 1, B = 32(int * 8)
    // Potential problem: Conflict misses
    // Possible solution: Blocking matrix

    int i, ii, j, jj;

    int a0, a1, a2, a3, a4, a5, a6, a7;

    if (N == 32) {      // 32 * 32
        // Conflict misses: 8 row
        // bsize = 8
        // A: row-wise  B: column-wise

        for (i = 0; i < 32; i += 8) {
            for (j = 0; j < 32; j += 8) {
                for (ii = i; ii < i + 8; ii++) {
                    a0 = A[ii][j+0];
                    a1 = A[ii][j+1];
                    a2 = A[ii][j+2];
                    a3 = A[ii][j+3];
                    a4 = A[ii][j+4];
                    a5 = A[ii][j+5];
                    a6 = A[ii][j+6];
                    a7 = A[ii][j+7];
                    B[j+0][ii] = a0;
                    B[j+1][ii] = a1;
                    B[j+2][ii] = a2;
                    B[j+3][ii] = a3;
                    B[j+4][ii] = a4;
                    B[j+5][ii] = a5;
                    B[j+6][ii] = a6;
                    B[j+7][ii] = a7;
                }
            }
        }
    }
    else if (N == 64) { // 64 * 64
        for (i = 0; i < 64; i += 8) {
            for (j = 0; j < 64; j += 8) {
                for (ii = i; ii < i + 4; ii++) {
                    a0 = A[ii][j+0];
                    a1 = A[ii][j+1];
                    a2 = A[ii][j+2];
                    a3 = A[ii][j+3];
                    a4 = A[ii][j+4];
                    a5 = A[ii][j+5];
                    a6 = A[ii][j+6];
                    a7 = A[ii][j+7];

                    B[j+0][ii] = a0;
                    B[j+1][ii] = a1;
                    B[j+2][ii] = a2;
                    B[j+3][ii] = a3;
                    B[j+0][ii+4] = a4;
                    B[j+1][ii+4] = a5;
                    B[j+2][ii+4] = a6;
                    B[j+3][ii+4] = a7;
                }
                for (jj = j; jj < j + 4; jj++) {
                    a0 = B[jj][i+4];
                    a1 = B[jj][i+5];
                    a2 = B[jj][i+6];
                    a3 = B[jj][i+7];
                    a4 = A[i+4][jj];
                    a5 = A[i+5][jj];
                    a6 = A[i+6][jj];
                    a7 = A[i+7][jj];

                    B[jj][i+4] = a4;
                    B[jj][i+5] = a5;
                    B[jj][i+6] = a6;
                    B[jj][i+7] = a7;
                    B[jj+4][i+0] = a0;
                    B[jj+4][i+1] = a1;
                    B[jj+4][i+2] = a2;
                    B[jj+4][i+3] = a3;
                }
                for (ii = i; ii < i + 4; ii++) {
                    a0 = A[ii+4][j+4];
                    a1 = A[ii+4][j+5];
                    a2 = A[ii+4][j+6];
                    a3 = A[ii+4][j+7];
                    B[j+4][ii+4] = a0;
                    B[j+5][ii+4] = a1;
                    B[j+6][ii+4] = a2;
                    B[j+7][ii+4] = a3;
                }
            }
        }
    }
    else if (N == 67) { // 67 * 61
        for (i = 0; i < 67; i += 16) {
            for (j = 0; j < 61; j += 16) {
                for (ii = i; ii < i+16 && ii < 67; ii++) {
                    for (jj = j; jj < j+16 && jj < 61; jj++) {
                        B[jj][ii] = A[ii][jj];
                    }
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

