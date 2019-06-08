//Resolver R = minA . (AL) + maxA . (AA) + promA . (UA)
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

#define PRINT 0

int N, T;
double promA = 0;
int maxA = 0;
int minA = 9999999;
int i,j,k;

int *A, *At, *AA, *AL, *UA, *Ufil, *Lcol, *R;

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
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%u\t", m[i * N + j]);
		}
		printf("\n");
	}
}

void max_min_prom_trans()
{	
	#pragma omp parallel for private(i,j) reduction(min:minA) reduction(max:maxA) reduction(+:promA)
	for (i = 0; i < N; i++)
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

	if (!PRINT)
		return;
	printf("\nMax = %d\nMin = %d\nPromedio=%.2f\n", maxA, minA, promA);
}

void mult_maxA_AA()
{
	int acc;
	//Mult
	#pragma omp parallel for private(i,j,k,acc)
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			//Atranspuesta se almacena por filas pero se trata como por columnas. Termina calculando A*A.
			for (k = 0; k < N; k++)
				acc += A[i * N + k] * At[k + j * N];
			AA[i * N + j] = acc * maxA;
		}
	if (!PRINT)
		return;
	printf("\nMatriz At\n");
	imprimir_x_filas(At);
	printf("\nMatriz maxA . AA\n");
	imprimir_x_filas(AA);
}

void mult_minA_AL()
{
	int acc;
	//Mult
	#pragma omp parallel for private(i,j,k,acc) schedule(dynamic, 1)
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			for (k = 0; k < N; k++)
				if (k >= j)
					acc += A[i * N + k] * Lcol[k + j * N - j * (j + 1) / 2];
			AL[i * N + j] = acc * minA;
		}

	if (!PRINT)
		return;
	printf("\nMatriz minA . AL\n");
	imprimir_x_filas(AL);
}

void mult_promA_UA()
{
	int acc;
	//Mult
	#pragma omp parallel for private(i,j,k,acc) schedule(dynamic,1)
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			for (k = 0; k < N; k++)
				if (k >= i)
					acc += Ufil[i * N + k - i * (i + 1) / 2] * At[k + j * N];
			UA[i * N + j] = acc * promA;
		}

	if (!PRINT)
		return;
	printf("\nMatriz promA . UA\n");
	imprimir_x_filas(UA);
}

void sumar_todo()
{
	#pragma omp parallel for private(i,j)
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
			R[i * N + j] = AL[i * N + j] + UA[i * N + j] + AA[i * N + j];

	if (!PRINT)
		return;
	printf("\nMatriz R\n");
	imprimir_x_filas(R);
}

void init_matrices()
{
	//Aloca memoria para las matrices
	A = malloc(sizeof(int) * N * N);
	AL = malloc(sizeof(int) * N * N);
	UA = malloc(sizeof(int) * N * N);
	At = malloc(sizeof(int) * N * N);
	AA = malloc(sizeof(int) * N * N);
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

int main(int argc, char *argv[])
{
	double timetick;

	if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0))
	{
		printf("\nUsar: %s N T\n\tN: Dimension de la matriz N x N\n\tT: Cantidad de threads.\n", argv[0]);
		exit(1);
	}
	omp_set_num_threads(T);

	init_matrices();

	timetick = dwalltime();

	max_min_prom_trans();
	mult_maxA_AA();
	mult_minA_AL();
	mult_promA_UA();
	sumar_todo();

	printf("Tiempo con N = %d T = %d >> %.4f seg.\n", N, T, dwalltime() - timetick);

	free(A);
	free(At);
	free(AA);
	free(AL);
	free(UA);
	free(Ufil);
	free(Lcol);
	free(R);
	return 1;
}
