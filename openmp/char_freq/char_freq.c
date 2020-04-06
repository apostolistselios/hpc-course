/**
 * A program that counts the character frequency in a file 
 * in parallel using the OpenMP library.
 * 
 * Apostolos Tselios, March 2020
 */

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 128
#define BASE 0

void print_character_frequency(int freq[]);
void compute_character_frequency(int thread_count, long file_size, char *buffer, int freq[]);

int main(int argc, char *argv[]) {
    
    double start, end;
    FILE *pfile;
    long file_size;
    char *buffer, *filename;
    size_t result;
    int i, j, thread_count, freq[N];

    if (argc != 3) {
        printf("Usage: %s <thread count> <file name>\n", argv[0]);
        return 1;
    }

    thread_count = strtol(argv[1], NULL, 10);
    filename = argv[2];
    pfile = fopen(filename, "rb");
    if (pfile == NULL) {printf("File error\n"); return 2;}

    // Obtain the file size.
    fseek(pfile, 0, SEEK_END);
    file_size = ftell(pfile);
    rewind(pfile);
    printf("File size is %ld\n", file_size);

    // Allocate memory to contain the file.
    buffer = (char*) malloc(file_size * sizeof(char));
    if (buffer == NULL) {printf("Memory error\n"); return 3;}

    // Copy the file into the buffer.
    result = fread(buffer, 1, file_size, pfile);
    if (result != file_size) {printf("Reading error\n"); return 4;}

    start = omp_get_wtime();
    compute_character_frequency(thread_count, file_size, buffer, freq);
    end = omp_get_wtime();

    print_character_frequency(freq);
    
    fclose(pfile);
    free(buffer);

    printf("Time used: %f\n", (end - start));

    return 0;
}

/**
 * Function: print_character_frequency
 * -----------------------------------
 * Prints the character frequency found in the file.
 * 
 * int freq[]: the array that holds the frequency of each character.
 */
void print_character_frequency(int freq[]) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%d = %d\n", i+BASE, freq[i]);
    }
}

/**
 * Function: compute_character_frequency
 * --------------------------------------
 * Computes the frequency of each character in the file.
 * 
 * int thread_count: the number of threads.
 * long file_size: the size of the file.
 * char *buffer: a buffer where the file is stored.
 * int freq[]: an array where the frequency of each character will be stored.
 */
void compute_character_frequency(int thread_count, long file_size, char *buffer, int freq[]) {
    int i;
    for (i = 0; i < N; i++) {
        freq[i] = 0;
    }

    #pragma omp parallel for num_threads(thread_count) \
            private(i) shared(freq, buffer, file_size)
    for (i = 0; i < file_size; i++) {
        #pragma omp critical
        freq[buffer[i] - BASE]++;
    }
}