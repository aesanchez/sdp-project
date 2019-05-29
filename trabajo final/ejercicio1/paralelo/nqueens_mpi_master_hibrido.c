#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define WORK_TAG 1
#define FINISH_TAG 2

void slave(void);
void master(void);

int rank; //my rank
unsigned int N;
unsigned int P; //numero de workers

int initial_row;
MPI_Status status;
unsigned int result;
unsigned int total = 0;
int flag = 0;

void attend_worker()
{
	//recibir resultados de cualquier worker
	MPI_Recv(&result, 1, MPI_UINT32_T, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	// printf("Soluciones encontradas por ID:%d >> %d \n", status.MPI_SOURCE, result);
	//sumo al valor total
	total += result;

	//mandarle mas trabajo
	MPI_Send(&initial_row, 1, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
	initial_row++;
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void recursive_queens(int index, unsigned int *queens, unsigned int *total_solutions)
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

	unsigned int *queens = malloc(sizeof(int) * N);

	MPI_Comm_size(MPI_COMM_WORLD, &P);

	initial_row = 0;
	//reparte el primer trabajo para cada worker.
	for (rank = 1; rank < P; ++rank)
	{
		MPI_Send(&initial_row, 1, MPI_INT, rank, WORK_TAG, MPI_COMM_WORLD);
		initial_row++;
	}

	while (initial_row < N)
	{
		//chequear si hay resultados de los workers
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (!flag) //no hay, asique me toca trabajar a mi
		{
			queens[0] = initial_row++;
			static unsigned int master_result;
			master_result = 0;
			recursive_queens(1, queens, &master_result);
			// printf("Soluciones encontradas por ID:0 >> %d \n", result);
			total += master_result;
			continue; //resetear el while
		}

		attend_worker();
	}

	//recibir trabajo de los restantes
	for (rank = 1; rank < P; ++rank)
	{
		MPI_Recv(&result, 1, MPI_UINT32_T, rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		// printf("Soluciones encontradas por ID:%d >> %d \n", status.MPI_SOURCE, result);
		//sumo al valor total
		total += result;
	}

	// finalizar todos los slaves
	for (rank = 1; rank < P; ++rank)
	{
		MPI_Send(0, 0, MPI_UINT32_T, rank, FINISH_TAG, MPI_COMM_WORLD);
	}

	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n\n", N, total, dwalltime() - timetick);
}

void slave()
{
	unsigned int *queens = malloc(sizeof(int) * N);

	for (;;)
	{
		MPI_Recv(&initial_row, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == FINISH_TAG)
			return;

		/* Start task */
		queens[0] = initial_row;
		result = 0;
		recursive_queens(1, queens, &result);
		// printf("Calculando %d y me dio %d\n", initial_row, result);

		/* End task */

		MPI_Send(&result, 1, MPI_UINT32_T, 0, 0, MPI_COMM_WORLD);
	}
}