/**
 * A program that implements the Count Sort Algorithm 
 * in parallel using the MPI library.
 * 
 * Apostolos Tselios, April 2020
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"

#define MASTER 0
#define RANDOM_MAX 100

void count_sort(int rank, int np, int array[], int size);
void check_args(int argc, char *argv[]);
void generate_random_array(int array[], int size);
void print_array(int rank, int array[], int size);

int main(int argc, char *argv[]) {

    double start_time, end_time;

    int rank, np; // np = number of processes
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    if (rank == MASTER) {
        check_args(argc, argv);
    }
    
    int size = strtol(argv[1], NULL, 10);
    int* array = (int*) malloc(size * sizeof(int));
    int* sorted = (int*) malloc(size * sizeof(int));

    if (rank == MASTER) {
        srand(time(0));
        printf("Generate random array to be sorted...\n");
        generate_random_array(array, size);
        printf("Array = ");
        print_array(rank, array, size);
        printf("Sorting...\n");

        start_time = MPI_Wtime();
    }
    
    MPI_Bcast(array, size, MPI_INT, MASTER, MPI_COMM_WORLD);

    count_sort(rank, np, array, size);
    // MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Reduce(array, sorted, size, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
    
    if (rank == MASTER) {
        printf("Sorted = ");
        print_array(rank, sorted, size);

        end_time = MPI_Wtime();
        printf("Took: %f\n", end_time - start_time);
    }
    
    free(array);
    free(sorted);

    MPI_Finalize();
    return 0;
}

/**
 * Function: count_sort
 * ---------------------
 * Implements the Count Sort Algorithm, sorts the array passed as an argument.
 * 
 * int rank: the rank  of the process that calls the function.
 * int np: the number of processes.
 * int array[]: the array to be sorted.
 * int size: the size of the array.
 */
void count_sort(int rank, int np, int array[], int size) {
    int i, j, count;
    int start = (rank * size) / np;
    int end = ((rank + 1) * size) / np;
    int* temp = (int*) calloc(size, sizeof(int));

    for (i = start; i < end; i++) {
        count = 0;
        for (j = 0; j < size; j++) {
            if (array[j] < array[i]) 
                count++;
            else if (array[j] == array[i] && j < i)
                count++;
        }
        temp[count] = array[i];
    }
    memcpy(array, temp, size * sizeof(int));
    free(temp);
}

/**
 * Function: check_args
 * ---------------------
 * checks if the program was called with the correct amount
 * of command-line arguments. If not exits with exit code 1.
 * 
 * int argc: the number of command-line arguments.
 * char *argv[]: array with the command-line arguments.
 */
void check_args(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: mpiexec -np <number of processes> %s <array size>\n", argv[0]);
        exit(1);
    }
}

/**
 * Function: generate_random_array
 * ---------------------------------
 * generates an array with random numbers between 0 and RAND_MAX.
 * 
 * int array[]: the array in which the random numbers will be stored.
 * int size: the size of the array.
 */
void generate_random_array(int array[], int size) {
    int i;
    for (i = 0; i < size; i++) {
        array[i] = rand() % RANDOM_MAX;
    }
}

/**
 * Function: print_array
 * ----------------------
 * Prints an array.
 * 
 * int rank: the rank of the process that calls the function.
 * int array[]: the array to be printed.
 * int size: the size of the array.
 */
void print_array(int rank, int array[], int size) {
    // printf("My rank = %d, size = %d\n", rank, size);
    int i;
    for (i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}