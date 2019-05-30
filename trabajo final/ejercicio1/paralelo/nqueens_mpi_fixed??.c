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

void recursive_queens_truncado(int index, int *queens, int *total_solutions, int col_final)
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
			if (index == col_final) // Era la ultima reina
			{
				total_solutions[0] += 1;
			}
			else // Sigo buscando
			{
				queens[index] = i;
				recursive_queens_truncado(index + 1, queens, total_solutions, col_final);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	N = atoi(argv[1]);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	if (rank == 0)
		master();
	else
		slave();
	MPI_Finalize();
}
int next_work(int *queens, int size)
{
	static int previously_found = 0;
	if(size == 1)
	{
		if(previously_found++ == 0)//primer iteracion
			return 1;
		queens[0]++;
		if(queens[0] == N)
			return 0;
		return 1;
	}
	if (previously_found)//si en un llamado previo encontro un valor, actualizar
	{ 
		if(size == 2) queens[1]++;
		else if(size == 1) queens[0]++;
		previously_found = 0;
	}
	//busco donde poner
	while (queens[0] < N)
	{
		while (queens[1] < N)
		{
			if ((queens[0] != queens[1]) && (queens[0] + 1 != queens[1]) && (queens[0] - 1 != queens[1]))
			{
				previously_found = 1;
				return 1; //encontre
			}	
			else
				queens[1]++;
		}
		queens[0]++;
		queens[1] = 0;
	}
	return 0;

}
void master()
{
	double timetick = dwalltime();

	int result = 0;
	int total = 0;
	MPI_Status status;

	int *queens;
	int size;
	if ((P - 1) <= N) //porque no tengo en cuenta el master en este caso
	{
		//divido trabajo cada 1 columna
		size = 1;
		queens = malloc(sizeof(int) * size);
		queens[0] = 0;
	}
	else
	{
		//divido trabajo cada 2 columnas
		size = 2;
		queens = malloc(sizeof(int) * size);
		queens[0] = 0;
		queens[1] = 0;
	}
	//divido por primer columna
	while (next_work(queens, size))
	{
		// if(size == 2) printf("Trabajo %d %d\n", queens[0], queens[1]);
		// else printf("Trabajo %d\n", queens[0]);
		//recibir solicitudes
		MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
		//mandarle mas trabajo
		MPI_Send(queens, size, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
	}

	//una vez que termine, mato a todos los slaves
	for (int r = 1; r < P; r++)
		MPI_Send(0, 0, MPI_INT, r, FINISH_TAG, MPI_COMM_WORLD);
	MPI_Reduce(&result, &total, 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);

	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n\n", N, total, dwalltime() - timetick);
	free(queens);
}

void slave()
{
	int result = 0;
	MPI_Status status;
	int *queens = malloc(sizeof(int) * N);
	int size;
	if ((P - 1) <= N) //porque no tengo en cuenta el master en este caso	
		size = 1;//divido trabajo cada 1 columna
	else	
		size = 2;//divido trabajo cada 2 columnas
	int iteraciones = 0;
	for (;;)
	{
		MPI_Send(0, 0, MPI_INT, MASTER, WORK_TAG, MPI_COMM_WORLD); //ask for work
		MPI_Recv(queens, size, MPI_INT, MASTER, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == FINISH_TAG)
			break;
		/* Start task */
		iteraciones++;
		recursive_queens(size, queens, &result);
	}
	//Devolver mi trabajo
	printf("ID:%d llamado %d>> #:%d\n", rank, iteraciones, result);
	MPI_Reduce(&result, &result, 1, MPI_INT, MPI_SUM, MASTER, MPI_COMM_WORLD);
	free(queens);
}