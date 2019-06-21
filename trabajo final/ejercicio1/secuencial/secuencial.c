//Resolver R = minA . (AL) + maxA . (AA) + promA . (UA)
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/**
 * PRINT = 1 realiza impresiones intermedias para debugeo.
 */
#define PRINT 0

int N;
double promA = 0;
int maxA = 0;
int minA = 9999999;
int i, j, k;
int *A, *At, *Ufil, *Lcol, *R;

void exportar_octave();
double dwalltime();
void imprimir_x_filas(int *);
void max_min_prom_trans();
void mult_maxA_AA();
void mult_minA_AL();
void mult_promA_UA();
void init_matrices();

int main(int argc, char *argv[])
{
	double timetick;

	if ((argc != 2) || ((N = atoi(argv[1])) <= 0))
	{
		printf("\nUsar: %s N\n  N: Dimension de la matriz N x N\n", argv[0]);
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

/**
 * Funcion utilizada para medir tiempos de ejecucion
 * @return timestamp.
 */
double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

/**
 * Realiza la impresion de la matriz almacenada por filas.
 * @param m matriz a imprimir.
 */
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

/**
 * Realiza el calculo de maxA, minA, promA y la transpuesta de A (At).
 */
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

/**
 * Realiza la operacion (maxA . AA) y almacena el resultado en R. Se utiliza la matriz transpuesta de A.
 */
void mult_maxA_AA()
{
	int acc;

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

/**
 * Realiza la operacion (minA . AL) y suma el resultado al valor actual de R.
 */
void mult_minA_AL()
{
	int acc;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			for (k = j; k < N; k++)
				acc += A[i * N + k] * Lcol[k + j * N - j * (j + 1) / 2];
			R[i * N + j] += acc * minA;
		}

	if (!PRINT)
		return;
	printf("\nMatriz R = maxA . AA + minA . AL\n");
	imprimir_x_filas(R);
}

/**
 * Realiza la operacion (promA . UA) y suma el resultado al valor actual de R. Se utiliza la matriz transpuesta de A.
 */
void mult_promA_UA()
{
	int acc;
	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++)
		{
			acc = 0;
			for (k = i; k < N; k++)
				acc += Ufil[i * N + k - i * (i + 1) / 2] * At[k + j * N];
			R[i * N + j] += acc * promA;
		}

	if (!PRINT)
		return;
	printf("\nMatriz R = maxA . AA + minA . AL + promA . UA\n");
	imprimir_x_filas(R);
}

/**
 * Alocacion de memoria de todas las matrices e inicializacion de las matrices de entrada A, Ufil y Lcol.
 */
void init_matrices()
{
	A = malloc(sizeof(int) * N * N);
	At = malloc(sizeof(int) * N * N);
	R = malloc(sizeof(int) * N * N);
	Ufil = malloc(sizeof(int) * (N * (N + 1)) / 2);
	Lcol = malloc(sizeof(int) * (N * (N + 1)) / 2);

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
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (j >= i)
				printf("%d\t", Ufil[i * N + j - i * (i + 1) / 2]);
			else
				printf("0\t");
		}
		printf("\n");
	}

	printf("\nMatriz L\n");
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (i >= j)
				printf("%d\t", Lcol[i + j * N - j * (j + 1) / 2]);
			else
				printf("0\t");
		}
		printf("\n");
	}
}

/**
 * Imprime expresion en formato Octave/Matlab para corroborar el resultado correcto de la operacion.
 */
void exportar_octave()
{
	printf("A = [");
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			printf("%u ", A[i * N + j]);
		}
		printf(";");
	}
	printf("];");

	printf("U = [");
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
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
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
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