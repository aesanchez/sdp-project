#include <mpi.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "valores_correctos.h"

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
int depth_col;

int work_finished = 0;

double dwalltime();
void get_queens(int, int *, int *, int);
int get_next_work(int *, int);
void get_queens_master(int, int *, int *, int *, int);
void calculate_workload_depth();

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);
	N = atoi(argv[1]);
	queens = malloc(sizeof(int) * N);

	if (rank == 0)
		master();
	else
		slave();
	free(queens);
	MPI_Finalize();
}

void master()
{
	unsigned long int total = 0;
	printf("\nCalculando para N=%d y np=%d\n", N, P);
	double timetick = dwalltime();

	calculate_workload_depth();

	int *q_workload = malloc(sizeof(int) * depth_col);
	q_workload[0] = 0;

	int unread_msg;
	while (get_next_work(q_workload, depth_col - 1))
	{
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &unread_msg, &status);
		if (!unread_msg) //no hay, asique me toca trabajar a mi
		{
			memcpy(queens, q_workload, sizeof(int) * depth_col); //copiar la parte inicial del trabajo
			get_queens_master(depth_col, queens, &result, q_workload, N - 1);
			iteraciones++;
		}
		else
		{
			//recibir solicitudes
			MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
			//mandarle mas trabajo
			MPI_Send(q_workload, depth_col, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
		}
	}
	//printf("ID:%d\tllamado %d veces >>\t%d\n", rank, iteraciones, result);

	for (int r = 1; r < P; r++) //una vez que termine, mato a todos los slaves
		MPI_Send(0, 0, MPI_INT, r, FINISH_TAG, MPI_COMM_WORLD);

	MPI_Reduce(&result, &total, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD); //junto sus resultados

	printf("Soluciones = %lu\t| Tiempo = %.4f\n", total, dwalltime() - timetick);
	if (valores_correctos[N] != total)
		printf("Solucion incorrecta.\nX\nX\nX\nX\nX\n");

	free(q_workload);
}

void slave()
{
	for (;;)
	{
		MPI_Send(0, 0, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD); //ask for work
		MPI_Recv(queens, N, MPI_INT, MASTER_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		if (status.MPI_TAG == FINISH_TAG)
			break;
		/* Start task */
		iteraciones++;
		MPI_Get_count(&status, MPI_INT, &depth_col);
		get_queens(depth_col, queens, &result, N - 1);
	}
	//Devolver mi trabajo
	// printf("ID:%d\tllamado %d veces >>\t%d\n", rank, iteraciones, result);
	MPI_Reduce(&result, &result, 1, MPI_INT, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
}

void get_queens(int col_start, int *queens, int *total_solutions, int col_final)
{
	int check;
	int j;
	int index = col_start;
	queens[col_start] = 0;
	while (1)
	{
		// for (int x = 0; x < index + 1; x++)
		// 	printf("\t%d ", queens[x]);
		// printf("\n");
		j = 0;
		check = 1;
		while (j < index && check)
		{
			if ((queens[j] == queens[index]) || (queens[j] == queens[index] - (j - index)) || (queens[j] == queens[index] + (j - index)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (index == col_final) // Era la ultima columna
				total_solutions[0]++;
			else // Sigo buscando
			{
				index++;
				queens[index] = 0;
				continue;
			}
		}
		//update indexes
		queens[index]++;
		if (queens[index] == N) //termino de analizar esta columna
		{
			if (index == col_start) //si era donde arranco, termine
				return;
			else
			{
				index--;
				queens[index]++;
				if (queens[index] == N)
				{
					if (index == col_start)
						return;
					else
					{
						index--;
						queens[index]++;
					}
				}
			}
		}
	}
}

void get_queens_master(int col_start, int *queens, int *total_solutions, int *q_workload, int col_final)
{
	int check;
	int j;
	int index = col_start;
	int unread_msg;
	queens[col_start] = 0;
	while (1)
	{
		if (P != 1 && !work_finished) //TODO: en el trabajo P nunca es 1, genera overhead creo
		{
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &unread_msg, &status);
			if (unread_msg)
			{
				if (get_next_work(q_workload, depth_col - 1))
				{
					//recibir solicitudes
					MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
					//mandarle mas trabajo
					MPI_Send(q_workload, depth_col, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
				}
			}
		}
		// for (int x = 0; x < index + 1; x++)
		// 	printf("\t%d ", queens[x]);
		// printf("\n");
		j = 0;
		check = 1;
		while (j < index && check)
		{
			if ((queens[j] == queens[index]) || (queens[j] == queens[index] - (j - index)) || (queens[j] == queens[index] + (j - index)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (index == col_final) // Era la ultima columna
				total_solutions[0]++;
			else // Sigo buscando
			{
				index++;
				queens[index] = 0;
				continue;
			}
		}
		//update indexes
		queens[index]++;
		if (queens[index] == N) //termino de analizar esta columna
		{
			if (index == col_start) //si era donde arranco, termine
				return;
			else
			{
				index--;
				queens[index]++;
				if (queens[index] == N)
				{
					if (index == col_start)
						return;
					else
					{
						index--;
						queens[index]++;
					}
				}
			}
		}
	}
}

int get_next_work(int *queens, int col_final)
{
	static int index = 0;
	static int update_flag = 0;
	int check;
	int j;

	while (!work_finished)
	{
		//update indexes
		if (update_flag)
		{
			update_flag = 0;
			queens[index]++;
			if (queens[index] == N) //termino de analizar esta columna
			{
				if (index == 0) //termine
				{
					work_finished = 1;
					return 0;
				}
				else
				{
					index--;
					queens[index]++;
					if (queens[index] == N)
					{
						if (index == 0) //termine
						{
							work_finished = 1;
							return 0;
						}
						else
						{
							index--;
							queens[index]++;
						}
					}
				}
			}
		}
		//check
		j = 0;
		check = 1;
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
				update_flag = 1; //quiero que a la vuelta, actualice
				return 1; //encontro una opcion
			}
			else // Sigo buscando
			{
				index++;
				queens[index] = 0;
			}
		}
		else
			update_flag = 1;
	}
	return 0;
}

// void calculate_workload_depth()
// {
// 	//calcular workload, osea profundidad del arbol a pasar
// 	depth_col = 1;
// 	int workload = N;
// 	int previous_workload = 0;
// 	while (workload < P && depth_col <= N)
// 	{
// 		workload = 0;
// 		depth_col++;
// 		get_queens(0, queens, &workload, (depth_col - 1));
// 		printf("C=%d y work=%d\n", depth_col, workload);
// 		if (workload <= previous_workload)
// 		{ // no tiene sentido seguir iterando
// 			workload = previous_workload;
// 			depth_col--;
// 			break;
// 		}
// 		previous_workload = workload;
// 	}
// 	printf("Quedo en C=%d y work=%d\n", depth_col, workload);
// }

void calculate_workload_depth()
{
	//calcular workload, osea profundidad del arbol a pasar
	int workload;
	if (N >= P * 1.5) //para que no se den casos por ejemplo que N = 10 y tengo P=9, lo que haria que uno solo ejecute 2 veces y el resto nada.
	{
		depth_col = 1;
		workload = N;
	}
	else if ((N - 1) * (N - 2) > P) //aca ya se balancea mas equitativamente, y si no llegara a hacerlo, el tiempo de espera seria menor que el del caso anterior
	{
		depth_col = 2;
		workload = (N - 1) * (N - 2); //real para N > 4
	}
	else
	{
		depth_col = 3;
		workload = -1; //indeterminado
	}
	printf("Quedo en C=%d y work=%d\n", depth_col, workload);
}

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}