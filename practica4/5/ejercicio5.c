// Realizar un algoritmo paralelo que ordene un vector de tamaño N por mezcla.
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define MASTER 0

void master(void);
void slave(void);
int check();
double dwalltime(void);

int *V, *aux, *Vtmp;

void sort_arrays_v2(int size, int *v1, int *v2, int * result)
{
	int i = 0;
	int v_index[2]= {0, 0};

	int min;
	int min_j;
	for (i = 0; i < size; i++)
	{
		min = 999;
		if (v1[v_index[0]] < min && v_index[0] != size/2)
		{
			min = v1[v_index[0]];
			min_j = 0;
		}			
		if (v2[v_index[1]] < min && v_index[1] != size/2)
		{
			min = v2[v_index[1]];
			min_j = 1;
		}
		result[i] = min;
		v_index[min_j]++;
	}
}
void sort_arrays(int start, int end, int *original, int *aux)
{
	int i = 0;
	int size = end - start + 1;
	int partition_size = size / 2;
	int partitions_index[2];

	for (i = 0; i < 2; i++)
		partitions_index[i] = start + partition_size * i;

	int min;
	int min_j;
	for (i = 0; i < size; i++)
	{
		min = 999;
		// buscar el minimo (siempre el primer valor) en todos los arreglos
		for (int j = 0; j < 2; j++)
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
		original[i + start] = aux[i];

}
void merge(int start, int end, int *original, int *aux)
{
	// printf("Merge Start %d End %d\n", start, end);
	int size = end - start + 1;
	if (size > 2)
	{
		merge(start, start + size / 2 - 1, original, aux); //izq
		merge(start + size / 2, end, original, aux);       //der
		//al volver aca, voy a tener dos tramos ordenados internamente, por lo que hay que juntarlos
		sort_arrays(start, end, original, aux);
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

int N;
int rank;
int P;

void imprimir(int *m, int size)
{
	for (int i = 0; i < size; i++)
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
	N = 1 << N;

	if (rank == 0)
		master();
	else
		slave();
	free(V);
	free(Vtmp);
	free(aux);
	MPI_Finalize();
	return EXIT_SUCCESS;
}
void work()
{
	Vtmp = malloc(sizeof(int) * N);
	for (int i = 2; i <= P; i *= 2)
	{
		if (rank % i == 0)
		{
			// aux = malloc(sizeof(int) * N / P * i);
			// Vtmp = malloc(sizeof(int) * N / P * i / 2);
			MPI_Recv(Vtmp, N / P * i / 2, MPI_INT, (rank + (i / 2)), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			sort_arrays_v2((N / P * i), V, Vtmp,aux);
			V = aux;
		}
		else
		{
			MPI_Send(V, N / P * i / 2, MPI_INT, (rank - (i / 2)), 0, MPI_COMM_WORLD);
			break;
		}
	}
}

void master()
{
	V = malloc(sizeof(int) * N);
	aux = malloc(sizeof(int) * N);
	for (int i = 0; i < N; i++)
		V[i] = rand() % 100;

	imprimir(V, N);

	double start = dwalltime();

	MPI_Scatter(V, N / P, MPI_INT, V, N / P, MPI_INT, MASTER, MPI_COMM_WORLD);
	merge(0, N / P - 1, V, aux);

	work();

	if (check(V))
		printf("Sort correcta\n");
	else
		printf("Sort incorrecta\n");
	// imprimir(V, N);
	printf("Tardo: %f\n", dwalltime() - start);
}

void slave()
{
	V = malloc(sizeof(int) * N);
	aux = malloc(sizeof(int) * N);

	MPI_Scatter(V, N / P, MPI_INT, V, N / P, MPI_INT, MASTER, MPI_COMM_WORLD);
	merge(0, N / P - 1, V, aux);

	work();
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

int check(int *V)
{
	int ok = 1;
	int i = 0;
	while (ok && i < N - 1)
	{
		if (V[i] > V[i + 1])
			ok = 0;
		i++;
	}
	return ok;
}