/**
 * A program that multiplies a matrix with a vector in parallel using MPI.
 * 
 * Apostolos Tselios, April 2020
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "mpi.h"

#define MASTER 0

void check_args(int argc, char *argv[]);
void generate_random_matrix(int size, double *matrix);
void generate_random_vector(int size, double *vector);
void multiply(int rank, int np, int size, double *matrix, double *vector, double *result_vector);
void output_result(int rank, int size, double *matrix, double *vector, double *result_vector);

int main(int argc, char *argv[]) {

  	int i, size;
  	double *matrix, *vector, *result_vector, *local_result_vector;
	
	int rank, np; // number of processes
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == MASTER) {check_args(argc, argv);}

	size = strtol(argv[1], NULL, 10);
	// Allocate memory for the matrix and vectors.
	matrix = (double*) malloc(size * size * sizeof(double));
	vector = (double*) malloc(size * sizeof(double));
	local_result_vector = (double*) malloc(size * sizeof(double));
	result_vector = (double*) malloc(size * sizeof(double));

	// Assign random values to matrix / vector.
	if (rank == MASTER) {
		srand(time(NULL));
		generate_random_matrix(size, matrix);
		generate_random_vector(size, vector);

		for (i = 0; i < size; i++) {
			result_vector[i] = 0.0;
		}
	}

	MPI_Bcast(matrix, size*size, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);
	MPI_Bcast(vector, size, MPI_DOUBLE, MASTER, MPI_COMM_WORLD);

	// computation
	multiply(rank, np, size, matrix, vector, local_result_vector);

	MPI_Reduce(local_result_vector, result_vector, size, MPI_DOUBLE, MPI_SUM, MASTER, MPI_COMM_WORLD);

	// output of data -- master
	if (rank == MASTER) {
		printf("RESULT\n");
		output_result(rank, size, matrix, vector, result_vector);
	}

	MPI_Finalize(); 		
	return 0;
}

/**
 * Function: check_args
 * ------------------------
 * Checks if the correct command line arguments were passed in.
 * 
 * int argc: the number of the command line arguments.
 * char *argv[]: an array that contains the command line arguments.
 */
void check_args(int argc, char *argv[]) {
	if (argc != 2) {
		printf("Usage : %s <matrix size>\n", argv[0]);
        exit(1);
	}
}

/**
 * Function: multiply
 * -----------------------
 * Multiplies a matrix with a vector.
 * 
 * int rank: the rank of the process that calls the function.
 * int np: the number of processes.
 * int size: the size of the matrix/vector.
 * double *matrix: a matrix.
 * double *vector: a vector.
 * double *result_vector: the result vector.
 */
void multiply(int rank, int np, int size, double *matrix, double *vector, double *result_vector) {
	int i, j;
	int start = (rank * size) / np;
    int end = ((rank + 1) * size) / np;

	for (i = 0; i < size; i++) {
		result_vector[i] = 0.0;
	}

	for (i = start; i < end; i++) {
		for (j = 0; j < size; j++) {
			result_vector[i] = result_vector[i] + matrix[i * size + j] * vector[j];
		}
	}
}

/**
 * Function: generate_random_matrix
 * ----------------------------------
 * Generates a matrix with random elements
 * 
 * int size: the size of the matrix.
 * double *matrix: the matrix
 */
void generate_random_matrix(int size, double *matrix) {
	int i, j;

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			// matrix[i * size + j] = 1.0;
			matrix[i * size + j] = (double) rand() / (RAND_MAX * 2.0 - 1.0);
		}
	}
}

/**
 * Function: generate_random_vector
 * --------------------------------
 * Generates an array/vector with random elements.
 * 
 * int size: the size of the vector.
 * double *vector: the array/vector.
 */
void generate_random_vector(int size, double *vector) {
	int i; 

	for (i = 0; i < size; i++) {
	    // vector[i] = 1.0; 
		vector[i] = (double) rand() / (RAND_MAX * 2.0 - 1.0);
    }
}

/**
 * Function: output_result
 * ------------------------
 * Outputs the result.
 * 
 * int rank: the rank of the process that calls the function.
 * int size: the size of the matrix/vectors.
 * double **matrix: a matrix.
 * double *vector: a vector.
 * double *result_vector: the result after matrix x vector.
 */
void output_result(int rank, int size, double *matrix, double *vector, double *result_vector) {
	int i, j;

	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			printf ("%1.3f ", matrix[i * size + j]);
		}
		printf("\t %1.3f ", vector[i]);
		printf("\t %1.3f \n", result_vector[i]);
		printf("\n");
	}
}