/*
Paralelizar la multiplicación de matrices cuadradas de NxN. Obtener el tiempo de
ejecución para N=512, 1024 y 2048. Ejecutar con 2 y 4 threads.
*/
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>

unsigned int N; //matris NxN
unsigned int T; //cantidad de threads
unsigned int *A;
unsigned int *B;
unsigned int *C;
int i,j,k, t;

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

typedef struct str
{
	int id;
	int offset;
	int filas;
} thread_args;

void crear_matrices(void);
void imprimir(unsigned int *);

void *thread_function(void *arg)
{
	thread_args my_args = *(thread_args *)arg;
	// printf("Id: %d / #Filas: %d / Offset: %d\n", my_args.id, my_args.filas, my_args.offset);	
	for (i = my_args.offset; i < (my_args.offset + my_args.filas); i++)
	{
		for (j = 0; j < N; j++)
		{
			double acc = 0;
			for (k = 0; k < N; k++)
			{
				acc += A[i * N + k] * B[k + j * N]; // A por filas y B por columnas
			}
			C[i * N + j] = acc; //x filas
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	N = atoi(argv[1]);
	T = atoi(argv[2]);

	crear_matrices();
	// imprimir(A);
	// imprimir(B);

	pthread_t *threads;
	threads = malloc(sizeof(pthread_t) * T);
	thread_args *args_array = malloc(sizeof(thread_args) * T);
	double timetick;
	timetick = dwalltime();
	for (t = 0; t < T; t++)
	{
		args_array[t].id = t;
		args_array[t].filas = N / T;
		args_array[t].offset = N / T * t;
		//dividir el trabajo en los hilos, dividiendo la matris resultado C.
		//division por bloques de filas.
		pthread_create(&threads[t], NULL, &thread_function, (void *)&args_array[t]);
	}
	for (t = 0; t < T; t++)
	{
		pthread_join(threads[t], NULL);
	}
	printf("Tiempo: %f \n", dwalltime() - timetick);
	// imprimir(C);

	return 0;
}

void crear_matrices()
{
	A = malloc(sizeof(unsigned int) * N * N);
	B = malloc(sizeof(unsigned int) * N * N);
	C = malloc(sizeof(unsigned int) * N * N);
	for (i = 0; i < N * N; i++)
	{
		A[i] = i;
		B[i] = i;
		C[i] = 0;
	}
}
void imprimir(unsigned int *m)
{
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%u\t", m[i * N + j]);
		}
		printf("\n");
	}
}