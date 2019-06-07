// Realizar un algoritmo paralelo que dado un vector V tamaño N obtenga el valor máximo, el valor mínimo y valor promedio de los elementos de V.

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define MASTER 0

void master(void);
void slave(void);
double dwalltime(void);

unsigned int N;
int rank;
unsigned int P;

unsigned int max = 0;
unsigned int localmax = 1;
unsigned int min =100;
unsigned int localmin = 100;
unsigned int sum =0;
unsigned int localsum = 0;


void imprimir(unsigned int *m)
{
	for (int i = 0; i < N; i++)
	{
		printf("%2.0u  ", m[i]);
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	N = atoi(argv[1]);
	
	              
	if (rank == 0)
		master();
	else
		slave();

	MPI_Finalize();
	return EXIT_SUCCESS;
}

void master()
{
	unsigned int * V = malloc(sizeof(unsigned int) * N);
	unsigned int * aux = malloc(sizeof(unsigned int) * N/P);
	

	for (unsigned int i = 0; i < N; i++)
		V[i] = rand() % 100;

	double start = dwalltime();

	MPI_Scatter(V, N/P, MPI_UNSIGNED, V, N/P, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);
	
	for(int i = 0; i< N/P; i++)
	{
	localsum+=V[i];
	if(V[i] > localmax)
		localmax = V[i];
		if(V[i] < localmin)
		localmin = V[i];
		
	}

    MPI_Reduce(&localmax, &max, 1, MPI_UNSIGNED, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&localmin, &min, 1, MPI_UNSIGNED, MPI_MIN, 0,MPI_COMM_WORLD);
    MPI_Reduce(&localsum, &sum, 1, MPI_UNSIGNED, MPI_SUM, 0,MPI_COMM_WORLD);

	printf("N: %d", N);
		printf("\n");
		printf("P: %d", P);
		printf("\n");
	imprimir(V);



   printf("Max: %d ", max);
	printf("\n");
   printf("Min: %d ", min);
	printf("\n");
	printf("Avg: %d ", sum/N);
	printf("\n");

	printf("Tardo: %f\n", dwalltime() - start);

	free(V);
	free(aux);
}

void slave()
{
	unsigned int * V = malloc(sizeof(unsigned int) * N/P);
	unsigned int *aux = malloc(sizeof(unsigned int) * N/P);

	MPI_Scatter(V, N/P, MPI_UNSIGNED, V, N/P, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);

	for(int i = 0; i< N/P; i++)
	{
		localsum+=V[i];
	if(V[i] > localmax)
		localmax = V[i];
		if(V[i] < localmin)
		localmin = V[i];
		
	}

    MPI_Reduce(&localmax, &max, 1, MPI_UNSIGNED, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&localmin, &min, 1, MPI_UNSIGNED, MPI_MIN, 0,MPI_COMM_WORLD);
	MPI_Reduce(&localsum, &sum, 1, MPI_UNSIGNED, MPI_SUM, 0,MPI_COMM_WORLD);
	

	free(V);
	free(aux);
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}