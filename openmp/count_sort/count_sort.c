/**
 * A program that implements the Count Sort Algorithm 
 * in parallel using the OpenMP library.
 * 
 * Apostolos Tselios, March 2020
 */

#include <omp.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RANDOM_MAX 100

void count_sort(int size, int thread_count, int array[], int sorted_arr[]);
void check_args(int argc, char *argv[]);
void generate_random_array(int size, int array[]);
void print_array(int size, int array[]);

int main(int argc, char *argv[]) {
    double start, end;

    srand(time(0));
    check_args(argc, argv);

    int thread_count = strtol(argv[1], NULL, 10);
    int size = strtol(argv[2], NULL, 10);
    int* array = (int*) malloc(size * sizeof(int));
    int* sorted_arr = (int*) malloc(size * sizeof(int));
    if (array == NULL || sorted_arr == NULL) {
        printf("Memory error: allocating memor for array or sorted_arr\n");
        return 2;
    }
    
    printf("Generate random array to be sorted...\n");
    generate_random_array(size, array);
    // print_array(size, array);

    start = omp_get_wtime();
    printf("Sorting...\n");
    count_sort(size, thread_count, array, sorted_arr);
    printf("Sorted...\n");
    start = omp_get_wtime();
    // print_array(size, sorted_arr);
    printf("Time used: %f\n", (end - start));
    
    free(array);
    free(sorted_arr);

    return 0;
}

/**
 * Function: count_sort
 * ---------------------
 * Implements the Count Sort Algorithm in parallel, sorts the array passed as an argument.
 * 
 * int size: the size of the array.
 * int array[]: the array to be sorted.
 */
void count_sort(int size, int thread_count, int array[], int sorted_arr[]) {
    int i, j, count;

    #pragma omp parallel for num_threads(thread_count) \
            private(i, j, count) shared(size, array, sorted_arr)
    for (i = 0; i < size; i++) {
        // int id = omp_get_thread_num();
        // printf("thread %d\n", id);
        count = 0;
        for (j = 0; j < size; j++) {
            if (array[j] < array[i]) 
                count++;
            else if (array[j] == array[i] && j < i)
                count++;
        }
        sorted_arr[count] = array[i];
    }
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
    if (argc != 3) {
        printf("Usage: %s <thread count> <array size>\n", argv[0]);
        exit(1);
    }
}

/**
 * Function: generate_random_array
 * ---------------------------------
 * generates an array with random numbers between 0 and RAND_MAX.
 * 
 * int size: the size of the array.
 * int thread_count: the number of threads to be created.
 * int array[]: the array in which the random numbers will be stored.
 */
void generate_random_array(int size, int array[]) {
    int i;
    for (i = 0; i < size; i++){
        array[i] = rand() % RANDOM_MAX;
    }
}

/**
 * Function: print_array
 * ----------------------
 * Prints an array.
 * 
 * int size: the size of the array.
 * int array[]: the array to be printed.
 */
void print_array(int size, int array[]) {
    int i;
    for (i = 0; i < size; i++){
        printf("%d ", array[i]);
    }
    printf("\n");
}