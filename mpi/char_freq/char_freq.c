/**
 * A program that counts the character frequency in a file 
 * in parallel using MPI.
 * 
 * Apostolos Tselios, April 2020
 */

#include <stdio.h> 
#include <stdlib.h> 

#include "mpi.h"

#define N 128
#define BASE 0
#define MASTER 0

void check_args(int argc, char *argv[]);
void print_character_frequency(int freq[]);
void compute_process_file_size(int rank, int np, long file_size, int* start, int* end, long* pfile_size);
void compute_character_frequency(long file_size, char *buffer, int *freq);

int main (int argc, char *argv[]) {
	
    FILE *pFile;
	// size_t result;
	int i, start, end; // start/end = the location in the file where each process will start/end counting.
	long file_size, pfile_size; // pfile_size = the size of the chunk of the file each process will handle.
	char *buffer, *filename;
    double start_time, end_time;

    int *freq = (int*) calloc(N, sizeof(int));
    int *total_freq = (int*) calloc(N, sizeof(int));
    
    int rank, np;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == MASTER) {
        check_args(argc, argv);
    }

    // open the file
	filename = argv[1];
	pFile = fopen ( filename , "rb" );
	if (pFile==NULL) {printf ("File error\n"); return 2;}

	// obtain file size
	fseek(pFile, 0, SEEK_END);
	file_size = ftell (pFile);
	rewind(pFile);
	
    if (rank == MASTER) {
        printf("File size = %ld\n", file_size);
        start_time = MPI_Wtime();
    }

    compute_process_file_size(rank, np, file_size, &start, &end, &pfile_size);
    // printf("rank = %d, start = %d, end = %d, pfile_size= %ld\n", rank, start, end, pfile_size);

	// allocate memory to contain the file
	buffer = (char*) malloc (pfile_size * sizeof(char));
	if (buffer == NULL) {printf ("Memory error\n"); return 3;}

    // sets the position for the process to start counting
    fseek(pFile, start, SEEK_SET);

	// copy the file into the buffer
	fread(buffer, 1, pfile_size, pFile);
	// if (result != pfile_size) {printf ("Reading error\n"); return 4;} 

	compute_character_frequency(pfile_size, buffer, freq);
    // MPI_Barrier(MPI_COMM_WORLD);

    MPI_Reduce(freq, total_freq, N, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
    
    if (rank == MASTER) {
	    print_character_frequency(total_freq);
        end_time = MPI_Wtime();
        printf("Took: %f\n", end_time - start_time);
    }

	fclose (pFile);
	free (buffer);

    MPI_Finalize();
	return 0;
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
		printf ("Usage: mpiexec -np <number of processes> %s <file_name>\n", argv[0]);
		exit(1);
    }
}

/**
 * Function: print_character_frequency
 * -----------------------------------
 * Prints the character frequency found in the file.
 * 
 * int *freq: the array that holds the frequency of each character.
 */
void print_character_frequency(int *freq) {
    int i;
    for (i = 0; i < N; i++) {
        printf("%c = %d\n", i+BASE, freq[i]);
        // printf("%d", freq[i]);
    }
    printf("\n");
}

/**
 * Function: compute_process_file_size
 * -----------------------------------
 * Computes the size of the file that corresponds to the process as well as 
 * the start and the end position of the process.
 * 
 * int rank: the rank of the process that calls the function.
 * int np: the number of processes.
 * long file_size: the size of the input file.
 * int *start: the start position of the process in the file.
 * int *end: the end position of the process in the file.
 * long *pfile_size: the size of the file that corresponds to the process.
 */
void compute_process_file_size(int rank, int np, long file_size, int *start, int *end, long *pfile_size) {
    *start = (rank * file_size) / np;
    *end = ((rank + 1) * file_size) / np;
    *pfile_size = *end - *start;
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
void compute_character_frequency(long file_size, char *buffer, int *freq) {
    int i;

    for (i = 0; i < file_size; i++) {
        freq[buffer[i] - BASE]++;
    }
}