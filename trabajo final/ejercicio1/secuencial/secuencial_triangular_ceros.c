//Resolver R = minA . (AL) + maxA . (AA) + promA . (UA)
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define PRINT 1

int N;
double promA = 0;
int maxA = 0;
int minA = 9999999;
int i, j, k;

int *A, *At, *Ufil, *Lcol, *R;

void exportar_octave(){
	printf("A = [");
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
			printf("%u ", A[i * N + j]);
		printf(";");
	}
	printf("];");

	printf("U = [");
	for (i = 0; i < N; i++) //fila
	{
		for (j = 0; j < N; j++)
			printf("%u ", Ufil[i * N + j]);
		printf(";");
	}
	printf("];");

	printf("L = [");
	for (i = 0; i < N; i++) //fila
	{
		for (j = 0; j < N; j++) //col
			printf("%d ", Lcol[i + N * j]);
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
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%u\t", m[i * N + j]);
		}
		printf("\n");
	}
}
void imprimir_x_col(int *m)
{
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%u\t", m[i + N * j]);
		}
		printf("\n");
	}
}

void max_min_prom_trans()
{
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
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			//Atranspuesta se almacena por filas pero se trata como por columnas. Termina calculando A*A.
			for (k = 0; k < N; k++)
				acc += A[i * N + k] * At[k + j * N];
			R[i * N + j] = acc * maxA;
		}
	if (!PRINT)
		return;
	printf("\nMatriz At\n");
	imprimir_x_filas(At);
	printf("\nMatriz R = maxA . AA\n");
	imprimir_x_filas(R);
}

void mult_minA_AL()
{
	int acc;
	//Mult
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			for (k = 0; k < N; k++)
				acc += A[i * N + k] * Lcol[k + j * N];
			R[i * N + j] += acc * minA;
		}

	if (!PRINT)
		return;
	printf("\nMatriz R = maxA . AA + minA . AL\n");
	imprimir_x_filas(R);
}

void mult_promA_UA()
{
	int acc;
	//Mult
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			for (k = 0; k < N; k++)
				acc += Ufil[i * N + k] * At[k + j * N];
			R[i * N + j] += acc * promA;
		}

	if (!PRINT)
		return;
	printf("\nMatriz R = maxA . AA + minA . AL + promA . UA\n");
	imprimir_x_filas(R);
}

void init_matrices()
{
	//Aloca memoria para las matrices
	A = malloc(sizeof(int) * N * N);
	At = malloc(sizeof(int) * N * N);
	Ufil = malloc(sizeof(int) * N * N);
	Lcol = malloc(sizeof(int) * N * N);
	R = malloc(sizeof(int) * N * N);

	for (i = 0; i < N * N; i++)
		A[i] = rand() % 10 + 1;

	for (i = 0; i < N; i++) //fila
	{
		for (j = 0; j < N; j++) //col
		{
			if (j >= i)
				Ufil[i * N + j] = rand() % 10 + 1;
			else
				Ufil[i * N + j] = 0;
		}
	}
	for (i = 0; i < N; i++) //fila
	{
		for (j = 0; j < N; j++) //col
		{
			if (i >= j)
				Lcol[i + N * j] = rand() % 10 + 1;
			else
				Lcol[i + N * j] = 0;
		}
	}

	if (!PRINT)
		return;

	printf("\nMatriz A\n");
	imprimir_x_filas(A);

	printf("\nMatriz U\n");
	imprimir_x_filas(Ufil);

	printf("\nMatriz L\n");
	imprimir_x_col(Lcol);
}

int main(int argc, char *argv[])
{
	double timetick;

	if ((argc != 2) || ((N = atoi(argv[1])) <= 0))
	{
		printf("\nUsar: %s n\n  n: Dimension de la matriz n x n\n", argv[0]);
		exit(1);
	}

	init_matrices();

	timetick = dwalltime();

	max_min_prom_trans();
	mult_maxA_AA();
	mult_minA_AL();
	mult_promA_UA();

	printf("Tiempo con N = %d >> %.4f seg.\n", N, dwalltime() - timetick);

	// exportar_octave(); //debug
	free(A);
	free(At);
	free(Ufil);
	free(Lcol);
	free(R);
	return EXIT_SUCCESS;
}
