//Resolver R = minA . (AL) + maxA . (AA) + promA . (UA)
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#define PRINT 0

int N, T;
double shared_promA = 0;
int shared_maxA = 0;
int shared_minA = 9999999;
int *A, *At, *Ufil, *Lcol, *R;
pthread_mutex_t lock;
pthread_barrier_t barrier;

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
		A[i] = rand()%10 + 1;

	for (i = 0; i < (N * (N + 1)) / 2; i++)
		Ufil[i] = rand()%10 + 1;

	for (i = 0; i < (N * (N + 1)) / 2; i++)
		Lcol[i] = rand()%10 + 1;

	if(!PRINT)
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

typedef struct str
{
	int id;
	int start;
	int end;
} t_args;

void * t_function(void *my_arg){
	t_args args = *(t_args *)my_arg;
	int i, j, k;
	int accAL, accAA, accUA;
	int maxA = 0;
	int minA = 9999999;
	double promA = 0;
	if(PRINT)
		printf("Id: %d / Start: %d / End:%d\n", args.id, args.start, args.end);

	//transponer A y calcular max,min,prom.
	for (i = args.start; i <= args.end; i++)
		for (j = 0; j < N; j++)
		{
			promA += A[i * N + j];
			if (A[i * N + j] > maxA)
				maxA = A[i * N + j];
			if (A[i * N + j] < minA)
				minA = A[i * N + j];
			At[i * N + j] = A[i + j * N];
		}
	promA = promA / (N * N);
	pthread_mutex_lock(&lock);
	shared_promA += promA;
	if(shared_maxA < maxA)
		shared_maxA = maxA;
	if(shared_minA > minA)
		shared_minA = minA;
	pthread_mutex_unlock(&lock);

	pthread_barrier_wait(&barrier);

	//Mult
	for (i = args.start; i <= args.end; i++)
		for (j = 0; j < N; j++)
		{
			accAL = 0;
			accAA = 0;
			accUA = 0;
			for (k = 0; k < N; k++){
				if (k >= j)
					accAL += A[i * N + k] * Lcol[k + j * N - j * (j + 1) / 2];
				if (k >= i)
					accUA += Ufil[i * N + k - i * (i + 1) / 2] * At[k + j * N];
				accAA += A[i * N + k] * At[k + j * N];
			}
			R[i * N + j] = accAL * shared_minA + accAA * shared_maxA + accUA * shared_promA;
		}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	double timetick;

	if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
	{
		printf("\nUsar: %s N T\n\tN: Dimension de la matriz N x N\n\tT: Cantidad de threads.\n", argv[0]);
		exit(1);
	}
	init_matrices();
	pthread_t * threads = malloc(sizeof(pthread_t) * T);
	t_args *args_array = malloc(sizeof(t_args) * T);
	pthread_mutex_init(&lock, NULL);
	pthread_barrier_init(&barrier,NULL,T);

	timetick = dwalltime();
	for (int t = 0; t < T; t++)
	{
		args_array[t].id = t;
		args_array[t].start = N / T * t;
		args_array[t].end =  N / T * t + N / T - 1;
		pthread_create(&threads[t], NULL, &t_function, (void *)&args_array[t]);
	}
	for (int i = 0; i < T; i++)
		pthread_join(threads[i], NULL);

	if(PRINT){
		printf("\nMax = %d\nMin = %d\nPromedio=%.2f\n", shared_maxA, shared_minA, shared_promA);
		printf("\nMatriz R\n");
		imprimir_x_filas(R);
	}
		
	printf("Tiempo con N = %d T = %d >> %.4f seg.\n", N, T, dwalltime() - timetick);

	free(A);
	free(At);
	free(Ufil);
	free(Lcol);
	free(R);
	pthread_mutex_destroy(&lock);
	pthread_barrier_destroy(&barrier);
	return 1;
}
