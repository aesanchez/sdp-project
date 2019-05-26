#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

void master(void);
void slave(void);
void multiply(void);
void imprimir(double *m);
int check(double *A, int n);
double dwalltime(void);
void finalSum(void);
void genMultiply(double *X, double *Y, double *Z);

unsigned int N; //matris NxN

double *A;
double *B;
double *AB;
double *C;
double *D;
double *CD;
double *E;
double *F;
double *EF;
double *R;

int rank;
MPI_Status *status;
MPI_Request *request;
int P;

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	N = atoi(argv[1]);

	if (rank == 0)
	{
		master();
	}
	else
	{
		slave();
	}
	MPI_Finalize();
	return 0;
}

void master()
{
	A = malloc(sizeof(double) * N * N);
	B = malloc(sizeof(double) * N * N);
	C = malloc(sizeof(double) * N * N);
	D = malloc(sizeof(double) * N * N);
	E = malloc(sizeof(double) * N * N);
	F = malloc(sizeof(double) * N * N);
	R = malloc(sizeof(double) * N * N);
	AB = malloc(sizeof(double) * N * N);
	CD = malloc(sizeof(double) * N * N);
	EF = malloc(sizeof(double) * N * N);

	for (int i = 0; i < N * N; i++)
	{
		A[i] = 1;
		B[i] = 1;
		C[i] = 1;
		D[i] = 1;
		E[i] = 1;
		F[i] = 1;
	}

	double start = dwalltime();

	MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, N * N / P, MPI_DOUBLE, A, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	genMultiply(A, B, AB);

	MPI_Gather(AB, N * N / P, MPI_DOUBLE, AB, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	// imprimir(C);
	if (check(AB, N))
		printf("Multiplicacion1 correcta\n");
	else
		printf("Multiplicacion1 incorrecta\n");

	MPI_Bcast(D, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(C, N * N / P, MPI_DOUBLE, C, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	genMultiply(C, D, CD);

	MPI_Gather(CD, N * N / P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (check(CD, N))
		printf("Multiplicacion2 correcta\n");
	else
		printf("Multiplicacion2 incorrecta\n");

	MPI_Bcast(F, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(E, N * N / P, MPI_DOUBLE, E, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	genMultiply(E, F, EF);

	MPI_Gather(EF, N * N / P, MPI_DOUBLE, EF, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (check(EF, N))
		printf("Multiplicacion3 correcta\n");
	else
		printf("Multiplicacion3 incorrecta\n");

	finalSum();

	printf("Tardo: %f\n", dwalltime() - start);
}

void finalSum()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			R[i * N + j] = AB[i * N + j] + CD[i * N + j] + EF[i * N + j];
	}
}
void slave()
{
	A = malloc(sizeof(double) * N * N / P);
	B = malloc(sizeof(double) * N * N);
	C = malloc(sizeof(double) * N * N / P);
	D = malloc(sizeof(double) * N * N);
	E = malloc(sizeof(double) * N * N / P);
	F = malloc(sizeof(double) * N * N);
	AB = malloc(sizeof(double) * N * N / P);
	CD = malloc(sizeof(double) * N * N / P);
	EF = malloc(sizeof(double) * N * N / P);

	MPI_Bcast(B, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(A, N * N / P, MPI_DOUBLE, A, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	genMultiply(A, B, AB);

	MPI_Gather(AB, N * N / P, MPI_DOUBLE, AB, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	MPI_Bcast(D, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(C, N * N / P, MPI_DOUBLE, C, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	genMultiply(C, D, CD);

	MPI_Gather(CD, N * N / P, MPI_DOUBLE, CD, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	MPI_Bcast(F, N * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(E, N * N / P, MPI_DOUBLE, E, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	genMultiply(E, F, EF);

	MPI_Gather(EF, N * N / P, MPI_DOUBLE, EF, N * N / P, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

void multiply()
{
	double acc = 0;
	for (int i = 0; i < N / P; i++) //fila
	{
		for (int j = 0; j < N; j++) //col
		{
			acc = 0;
			for (int k = 0; k < N; k++)
			{
				acc += A[i * N + k] * B[k + j * N]; // A por filas y B por columnas
			}
			AB[i * N + j] = acc; //x filas
		}
	}
}

void genMultiply(double *X, double *Y, double *Z)
{
	double acc = 0;
	for (int i = 0; i < N / P; i++) //fila
	{
		for (int j = 0; j < N; j++) //col
		{
			acc = 0;
			for (int k = 0; k < N; k++)
			{
				acc += X[i * N + k] * Y[k + j * N]; // X por filas e Y por columnas
			}
			Z[i * N + j] = acc; //x filas
		}
	}
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void imprimir(double *m)
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			printf("%.0f\t", m[i * N + j]);
		}
		printf("\n");
	}
}
int check(double *A, int n)
{
	int ok = 1;
	for (int i = 0; i < n * n; i++)
		ok = ok && (A[i] == n);
	return ok;
}