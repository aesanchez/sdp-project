#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#define WORK_TAG 1
#define FINISH_TAG 2
#define MASTER_RANK 0

void slave(void);
void master(void);

int rank; //my rank
int N;
int P; //numero de workers

int result = 0;
MPI_Status status;
int *queens;
int iteraciones = 0;
int num_col;

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
	MPI_Comm_size(MPI_COMM_WORLD, &P);
	N = atoi(argv[1]);
	queens = malloc(sizeof(int) * N);

	//calcular workload, osea profundidad del arbol a pasar
	//TODO: lo hacen todos. esta mal?
	num_col = 0;
	int workload = 0;
	while (workload < P)
	{
		recursive_queens_truncado(0, queens, &workload, num_col++);
		if (rank == 0)
			printf("C=%d y work=%d\n", num_col, workload);
	}

	if (rank == 0)
		master();
	else
		slave();
	free(queens);
	MPI_Finalize();
}

// int next_work(int *queens, int size)
// {
// 	static int previously_found = 0;
// 	if (size == 1)
// 	{
// 		if (previously_found++ == 0) //primer iteracion
// 			return 1;
// 		queens[0]++;
// 		if (queens[0] == N)
// 			return 0;
// 		return 1;
// 	}
// 	if (previously_found) //si en un llamado previo encontro un valor, actualizar
// 	{
// 		queens[size - 1]++;
// 		previously_found = 0;
// 	}
// 	//busco donde poner
// 	while (queens[0] < N)
// 	{
// 		while (queens[1] < N)
// 		{
// 			if ((queens[0] != queens[1]) && (queens[0] + 1 != queens[1]) && (queens[0] - 1 != queens[1]))
// 			{
// 				previously_found = 1;
// 				return 1; //encontre
// 			}
// 			else
// 				queens[1]++;
// 		}
// 		queens[0]++;
// 		queens[1] = 0;
// 	}
// 	return 0;
// }

int next_work_recursive(int *queens, int col_final)
{
	static int index = 0;
	static int first_time = 1;

	if (first_time)
	{
		first_time = 0;
		queens[index] = 0;
	}
	else
	{
		while (index >= 0)
		{
			queens[index]++;
			if (queens[index] == N)
			{
				queens[index] = 0;
				if (index == 0)
					return 0;
				else
					index--;
			}
			else
			{
				break;
			}
		}
	}

	// printf("id>>%d\n", index);

	while (queens[index] < N)
	{
		// printf("\tid>>%d\n", index);
		// for (int x = 0; x < index+1; x++)
		// 	printf("%d ", queens[x]);
		// printf("\n");

		int j = 0;
		int check = 1;
		while (j < index && check)
		{
			if ((queens[j] == queens[index]) || (queens[j] == queens[index] - (j - index)) || (queens[j] == queens[index] + (j - index)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (index == col_final) // Era la ultima columna
			{
				// for (int x = 0; x < num_col; x++)
				// 	printf("\t%d ", queens[x]);
				// printf("\n");
				return 1; //encontro una opcion
			}
			else // Sigo buscando
			{
				index++;
				first_time = 1;
				return next_work_recursive(queens, col_final);
			}
		}
		while (index >= 0)
		{
			queens[index]++;
			if (queens[index] == N)
			{
				queens[index] = 0;
				if (index == 0)
					return 0;
				else
					index--;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

void master()
{
	double timetick = dwalltime();
	int total = 0;
	int *q_workload;

	q_workload = malloc(sizeof(int) * 4);
	for (int x = 0; x < 4; x++)
		q_workload[x] = 0;

	int unread_msg;

	while (next_work_recursive(q_workload, num_col-1))
	{
		// for (int x = 0; x < num_col; x++)
		// 	printf("\t%d ", q_workload[x]);
		// printf("\n");
		// chequear si hay solicitudes de los workers
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &unread_msg, &status);
		if (!unread_msg) //no hay, asique me toca trabajar a mi
		{
			memcpy(queens, q_workload, sizeof(int) * num_col);
			recursive_queens(num_col, queens, &result);
			iteraciones++;
		}
		else
		{
			//recibir solicitudes
			MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
			//mandarle mas trabajo
			MPI_Send(q_workload, num_col, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
		}
	}
	printf("ID:%d llamado %d veces >> %d\n", rank, iteraciones, result);

	//una vez que termine, mato a todos los slaves
	for (int r = 1; r < P; r++)
		MPI_Send(0, 0, MPI_INT, r, FINISH_TAG, MPI_COMM_WORLD);
	//junto sus resultados
	MPI_Reduce(&result, &total, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);

	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n\n", N, total, dwalltime() - timetick);
	free(q_workload);
}

void slave()
{
	for (;;)
	{
		MPI_Send(0, 0, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD); //ask for work
		MPI_Recv(queens, num_col, MPI_INT, MASTER_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == FINISH_TAG)
			break;
		/* Start task */
		iteraciones++;
		recursive_queens(num_col, queens, &result);
	}
	//Devolver mi trabajo
	printf("ID:%d llamado %d veces >> %d\n", rank, iteraciones, result);
	MPI_Reduce(&result, &result, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
}