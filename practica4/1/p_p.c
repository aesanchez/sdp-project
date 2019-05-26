#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

#define TAG 69

void master(void);
void slave(void);
void multiply(void);
void imprimir(double *m);
int check(double *A, int n);
double dwalltime(void);

unsigned int N; //matris NxN

double *A;
double *B;
double *C;

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

	MPI_Request requests[2 * (P - 1)];
	MPI_Request *req = requests;
	for (int i = 0; i < N * N; i++)
	{
		A[i] = 1;
		B[i] = 1;
	}

	double start = dwalltime();

	for (int i = 1; i < P; i++)
	{
		MPI_Isend(&A[N * N / P * i], (N * N) / P, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, req++);
		MPI_Isend(B, N * N, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, req++);
	}

	multiply();

	req = requests;
	for (int i = 1; i < P; i++)
	{
		MPI_Irecv(&C[N * N / P * i], (N * N) / P, MPI_DOUBLE, i, TAG, MPI_COMM_WORLD, req++);
	}
	MPI_Waitall(P - 1, requests, MPI_STATUSES_IGNORE);

	// imprimir(C);
	if (check(C, N))
		printf("Multiplicacion correcta\n");
	else
		printf("Multiplicacion incorrecta\n");
	printf("Tardo: %f\n", dwalltime() - start);
}

void slave()
{
	A = malloc(sizeof(double) * N * N / P);
	B = malloc(sizeof(double) * N * N);
	C = malloc(sizeof(double) * N * N / P);

	MPI_Recv(A, (N * N) / P, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, status);
	MPI_Recv(B, N * N, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD, status);

	multiply();

	MPI_Send(C, (N * N) / P, MPI_DOUBLE, 0, TAG, MPI_COMM_WORLD);
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
			C[i * N + j] = acc; //x filas
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