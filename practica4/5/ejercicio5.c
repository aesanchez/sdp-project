// Realizar un algoritmo paralelo que ordene un vector de tamaño N por mezcla.
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>
#include <math.h>

#define MASTER 0

void master(void);
void slave(void);
int check();
double dwalltime(void);

void sort_arrays(int start, int end, int partitions, unsigned int * original, unsigned int * aux)
{
	int i = 0;
	int size = end - start + 1;
	int partition_size = size / partitions;
	int *partitions_index = malloc(sizeof(int) * partitions);

	for (i = 0; i < partitions; i++)
	{
		partitions_index[i] = start + partition_size * i;
	}

	unsigned int min;
	int min_j;
	for (i = 0; i < size; i++)
	{
		min = 999;
		// buscar el minimo (siempre el primer valor) en todos los arreglos
		for (int j = 0; j < partitions; j++)
		{
			if (partitions_index[j] != start + partition_size * (j + 1))
			{
				if (original[partitions_index[j]] < min)
				{
					min = original[partitions_index[j]];
					min_j = j;					
				}
			}
		}
		aux[i] = min;
		partitions_index[min_j]++;
	}
	// pasar todo al array original
	for (i = 0; i < size; i++)
	{
		original[i + start] = aux[i];
	}
	free(partitions_index);
}
void merge(int start, int end, unsigned int * original, unsigned int * aux)
{
	// printf("Merge Start %d End %d\n", start, end);
	int size = end - start + 1;
	if (size > 2)
	{
		merge(start, start + size / 2 - 1, original, aux); //izq
		merge(start + size / 2, end, original, aux);       //der
		//al volver aca, voy a tener dos tramos ordenados internamente, por lo que hay que juntarlos
		sort_arrays(start, end, 2, original, aux);
	}
	else
	{
		if (size == 1)
			return;
		//size=2
		//ordenar
		if (original[start] > original[end])
		{
			//swap
			int temp = original[start];
			original[start] = original[end];
			original[end] = temp;
		}
	}
}

unsigned int N;
int rank;
unsigned int P;

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
	N = pow(2,N);

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

	imprimir(V);

	double start = dwalltime();

	MPI_Scatter(V, N/P, MPI_UNSIGNED, V, N/P, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);
	
	merge(0, N/P-1, V, aux);

	MPI_Gather(V, N/P, MPI_UNSIGNED, V, N/P, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);
	sort_arrays(0, N-1, P, V, aux);

	if (check(V))
		printf("Multiplicacion correcta\n");
	else
		printf("Multiplicacion incorrecta\n");
	imprimir(V);
	printf("Tardo: %f\n", dwalltime() - start);

	free(V);
	free(aux);
}

void slave()
{
	unsigned int * V = malloc(sizeof(unsigned int) * N/P);
	unsigned int *aux = malloc(sizeof(unsigned int) * N/P);

	MPI_Scatter(V, N/P, MPI_UNSIGNED, V, N/P, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);

	merge(0, N/P-1, V, aux);

	MPI_Gather(V, N/P, MPI_UNSIGNED, V, N/P, MPI_UNSIGNED, MASTER, MPI_COMM_WORLD);
	
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

int check(unsigned int * V)
{
	int ok = 1;
	unsigned int i = 0;
	while(ok && i < N-1)
	{
		if(V[i]>V[i+1])
			ok = 0;
		i++;
	}
	return ok;
}