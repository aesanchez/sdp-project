//Resolver R = minA . (AL) + maxA . (AA) + promA . (UA)
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>

#define PRINT 0

typedef struct str
{
	int id;
	int start;
	int end;
} t_args;

int N, T;
sem_t *semaphores;
int *minA, *maxA;
float *promA;
int *A, *At, *Ufil, *Lcol, *R;
pthread_barrier_t barrier;
pthread_t *threads;
t_args *args_array;

void exportar_octave(){
	printf("A = [");
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%u ", A[i * N + j]);
		}
		printf(";");
	}
	printf("];");

	printf("U = [");
	for (int i = 0; i < N; i++) //fila
	{
		for (int j = 0; j < N; j++) //col
		{
			if (j >= i)
				printf("%d ", Ufil[i * N + j - i * (i + 1) / 2]);
			else
				printf("0 ");
		}
		printf(";");
	}
	printf("];");

	printf("L = [");
	for (int i = 0; i < N; i++) //fila
	{
		for (int j = 0; j < N; j++) //col
		{
			if (i >= j)
				printf("%d ", Lcol[i + j * N - j * (j + 1) / 2]);
			else
				printf("0 ");
		}
		printf(";");
	}
	printf("];");

	printf("R = min(min(A))* A*L + max(max(A)) * A*A + mean(mean(A)) *U*A");
	printf("\n");
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void imprimir_x_filas(int *m)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%u\t", m[i * N + j]);
		}
		printf("\n");
	}
}

void init_matrices()
{
	int i, j, k;
	//Aloca memoria para las matrices
	A = malloc(sizeof(int) * N * N);
	At = malloc(sizeof(int) * N * N);
	Ufil = malloc(sizeof(int) * (N * (N + 1)) / 2);
	Lcol = malloc(sizeof(int) * (N * (N + 1)) / 2);
	R = malloc(sizeof(int) * N * N);

	for (i = 0; i < N * N; i++)
		A[i] = rand() % 10 + 1;

	for (i = 0; i < (N * (N + 1)) / 2; i++)
		Ufil[i] = rand() % 10 + 1;

	for (i = 0; i < (N * (N + 1)) / 2; i++)
		Lcol[i] = rand() % 10 + 1;

	if (!PRINT)
		return;

	printf("\nMatriz A\n");
	imprimir_x_filas(A);

	printf("\nMatriz U\n");
	for (i = 0; i < N; i++) //fila
	{
		for (j = 0; j < N; j++) //col
		{
			if (j >= i)
				printf("%d\t", Ufil[i * N + j - i * (i + 1) / 2]);
			else
				printf("0\t");
		}
		printf("\n");
	}

	printf("\nMatriz L\n");
	for (i = 0; i < N; i++) //fila
	{
		for (j = 0; j < N; j++) //col
		{
			if (i >= j)
				printf("%d\t", Lcol[i + j * N - j * (j + 1) / 2]);
			else
				printf("0\t");
		}
		printf("\n");
	}
}

void *t_function(void *my_arg)
{
	t_args args = *(t_args *)my_arg;
	if (PRINT)
		printf("Id: %d / Start: %d / End:%d\n", args.id, args.start, args.end);
	int i, j, k;
	int accAL, accAA, accUA;
	maxA[args.id] = 0;
	minA[args.id] = 9999999;
	promA[args.id] = 0;

	//transponer A y calcular max,min,prom.
	for (i = args.start; i <= args.end; i++)
		for (j = 0; j < N; j++)
		{
			promA[args.id] += A[i * N + j];
			if (A[i * N + j] > maxA[args.id])
				maxA[args.id] = A[i * N + j];
			if (A[i * N + j] < minA[args.id])
				minA[args.id] = A[i * N + j];
			At[i * N + j] = A[i + j * N];
		}
	promA[args.id] /= (N * N);

	int aux, offset = 0;
	for (int i = 2; i <= T; i *= 2)
	{
		if (args.id % i == 0)
		{
			aux = args.id + i / 2;
			sem_wait(&semaphores[args.id / i] + offset); //espero a tener los dos valores
			if (minA[args.id] > minA[aux])
				minA[args.id] = minA[aux];
			if (maxA[args.id] < maxA[aux])
				maxA[args.id] = maxA[aux];
			promA[args.id] += promA[aux];
			offset += T / i;
		}
		else
		{
			sem_post(&semaphores[(args.id - (i / 2)) / i] + offset); //aviso que tengo mi valor disponible
			break;
		}
	}

	pthread_barrier_wait(&barrier);

	//Mult
	for (i = args.start; i <= args.end; i++)
		for (j = 0; j < N; j++)
		{
			accAL = 0;
			accAA = 0;
			accUA = 0;
			for (k = 0; k < N; k++)
			{
				if (k >= j)
					accAL += A[i * N + k] * Lcol[k + j * N - j * (j + 1) / 2];
				if (k >= i)
					accUA += Ufil[i * N + k - i * (i + 1) / 2] * At[k + j * N];
				accAA += A[i * N + k] * At[k + j * N];
			}
			R[i * N + j] = accAL * minA[0] + accAA * maxA[0] + accUA * promA[0];
		}

	pthread_exit(NULL);
}
void init_variables()
{
	semaphores = malloc(sizeof(sem_t) * T - 1);
	for (int i = 0; i < T - 1; i++)
		sem_init(&semaphores[i], 0, 0);
	minA = malloc(sizeof(int) * T);
	maxA = malloc(sizeof(int) * T);
	promA = malloc(sizeof(float) * T);
	threads = malloc(sizeof(pthread_t) * T);
	args_array = malloc(sizeof(t_args) * T);
	pthread_barrier_init(&barrier, NULL, T);
}

int main(int argc, char *argv[])
{
	double timetick;

	if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
	{
		printf("\nUsar: %s N T\n\tN: Dimension de la matriz N x N\n\tT: Cantidad de threads.\n", argv[0]);
		exit(1);
	}
	init_variables();
	init_matrices();

	timetick = dwalltime();
	for (int t = 0; t < T; t++)
	{
		args_array[t].id = t;
		args_array[t].start = N / T * t;
		args_array[t].end = N / T * t + N / T - 1;
		pthread_create(&threads[t], NULL, &t_function, (void *)&args_array[t]);
	}
	for (int i = 0; i < T; i++)
		pthread_join(threads[i], NULL);

	if (PRINT)
	{
		printf("\nMax = %d\nMin = %d\nPromedio=%.2f\n", maxA[0], minA[0], promA[0]);
		printf("\nMatriz R\n");
		imprimir_x_filas(R);
	}

	printf("Tiempo con N = %d T = %d >> %.4f seg.\n", N, T, dwalltime() - timetick);

	//exportar_octave();
	free(A);
	free(At);
	free(Ufil);
	free(Lcol);
	free(R);
	pthread_barrier_destroy(&barrier);
	for (int i = 0; i < T - 1; i++)
		sem_destroy(&semaphores[i]);
	return 1;
}
