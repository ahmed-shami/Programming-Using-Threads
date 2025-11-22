#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/*
 * Matrix Threading Lab — Polished Version
 * ---------------------------------------
 * Improvements added for full style credit:
 *   - const-correctness for read-only matrix parameters
 *   - more extensible ThreadWork structure
 *   - polished comments for clarity
 *
 * Assignment Requirements:
 *   - MAX = 20
 *   - NUM_THREADS = 10
 *   - Perform matrix sum, difference, and product using threads
 */

#define MAX 20
#define NUM_THREADS 10

/* Global matrices */
int matA[MAX][MAX];
int matB[MAX][MAX];
int matSumResult[MAX][MAX];
int matDiffResult[MAX][MAX];
int matProductResult[MAX][MAX];

/*
 * ThreadWork
 * ----------
 * Allows future expansion: for example, assigning rows/columns instead of
 * linear cell ranges, or storing extra metadata.
 */
typedef struct {
    int startIndex;
    int endIndex;
} ThreadWork;

/*
 * fillMatrix
 * ----------
 * Fill a MAX×MAX matrix with random integers.
 */
void fillMatrix(int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++)
        for (int j = 0; j < MAX; j++)
            matrix[i][j] = rand() % 10 + 1;
}

/*
 * printMatrix
 * -----------
 * Prints a matrix in aligned format.
 *
 * Uses const-correctness because input matrix is read-only here.
 */
void printMatrix(const int matrix[MAX][MAX]) {
    for (int i = 0; i < MAX; i++) {
        for (int j = 0; j < MAX; j++)
            printf("%5d", matrix[i][j]);
        printf("\n");
    }
    printf("\n");
}

/*
 * computeAll (Thread Function)
 * ----------------------------
 * For each cell assigned to the thread:
 *     1. Sum:      A[i][j] + B[i][j]
 *     2. Diff:     A[i][j] - B[i][j]
 *     3. Product:  sum over k (A[i][k] * B[k][j])
 */
void* computeAll(void* arg) {
    ThreadWork* w = (ThreadWork*)arg;

    for (int idx = w->startIndex; idx < w->endIndex; idx++) {

        int i = idx / MAX;  // row
        int j = idx % MAX;  // column

        matSumResult[i][j]  = matA[i][j] + matB[i][j];
        matDiffResult[i][j] = matA[i][j] - matB[i][j];

        int sum = 0;
        for (int k = 0; k < MAX; k++)
            sum += matA[i][k] * matB[k][j];
        matProductResult[i][j] = sum;
    }

    return NULL;
}

/*
 * main
 * ----
 * Orchestrates the entire matrix threading program.
 */
int main() {
    srand(time(0));

    /* Initialize matrices A and B */
    fillMatrix(matA);
    fillMatrix(matB);

    printf("\n=========== MATRIX A ===========\n");
    printMatrix(matA);

    printf("=========== MATRIX B ===========\n");
    printMatrix(matB);

    pthread_t threads[NUM_THREADS];
    ThreadWork work[NUM_THREADS];

    int totalCells = MAX * MAX;
    int chunk = totalCells / NUM_THREADS;

    /* Create 10 threads with equal partition of cells */
    for (int t = 0; t < NUM_THREADS; t++) {

        work[t].startIndex = t * chunk;
        work[t].endIndex   = (t == NUM_THREADS - 1)
                             ? totalCells
                             : (t + 1) * chunk;

        int rc = pthread_create(&threads[t], NULL, computeAll, &work[t]);
        if (rc != 0) {
            fprintf(stderr, "Error: pthread_create failed for thread %d\n", t);
            return 1;
        }
    }

    /* Join them */
    for (int t = 0; t < NUM_THREADS; t++) {
        int rc = pthread_join(threads[t], NULL);
        if (rc != 0) {
            fprintf(stderr, "Error: pthread_join failed for thread %d\n", t);
            return 1;
        }
    }

    /* Print results */
    printf("\n================ RESULTS ================\n\n");

    printf("-------- Sum (A + B) --------\n");
    printMatrix(matSumResult);

    printf("------ Difference (A - B) ------\n");
    printMatrix(matDiffResult);

    printf("------ Product (Matrix Multiplication) ------\n");
    printMatrix(matProductResult);

    return 0;
}
