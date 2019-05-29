#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define WORK_TAG 1
#define FINISH_TAG 2
#define MASTER 0

void slave(void);
void master(void);

int rank; //my rank
int N;
int P; //numero de workers

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void recursive_queens(int index, int *queens, int *total_solutions)
{
	for (int i = 0; i < N; i++)
	{
		// Inicio de checkeo
		int check = 1;
		int j = 0;
		while (j < index && check)
		{
			if ((queens[j] == i) || (queens[j] == i - (j - index)) || (queens[j] == i + (j - index)))
				check = 0;
			j++;
		}
		// Fin de checkeo
		if (check)
		{
			if (index + 1 == N) // Era la ultima reina
			{
				total_solutions[0] += 1;
			}
			else // Sigo buscando
			{
				queens[index] = i;
				recursive_queens(index + 1, queens, total_solutions);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	N = atoi(argv[1]);

	if (rank == 0)
		master();
	else
		slave();
	MPI_Finalize();
}

void master()
{
	double timetick = dwalltime();

	int initial_row=0;
	int result;
	int total = 0;
	MPI_Status status;

	MPI_Comm_size(MPI_COMM_WORLD, &P);

	while (initial_row < N)
	{
		//recibir solicitudes
		MPI_Recv(0,0,MPI_INT,MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
		//mandarle mas trabajo
		MPI_Send(&initial_row, 1, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
		initial_row++;
	}
	//una vez que termine, mato a todos los slaves
	for (int r = 1; r < P; r++)
		MPI_Send(0,0,MPI_INT, r, FINISH_TAG, MPI_COMM_WORLD);
	MPI_Reduce(&result, &total,  1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n\n", N, total, dwalltime() - timetick);
}

void slave()
{
	int result = 0;
	int initial_row;
	MPI_Status status;
	int * queens = malloc(sizeof(int) * N);

	for (;;)
	{
		MPI_Send(0,0,MPI_INT,MASTER, WORK_TAG, MPI_COMM_WORLD); //ask for work
		MPI_Recv(&initial_row, 1, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == FINISH_TAG)
			break;	
		/* Start task */
		queens[0] = initial_row;
		recursive_queens(1, queens, &result);
	}
	//Devolver mi trabajo
	printf("(%d)Devuelvo %d\n", rank, result);
	MPI_Reduce(&result, &result,  1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
}