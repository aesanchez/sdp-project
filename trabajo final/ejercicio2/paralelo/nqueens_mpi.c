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

int N, P;
int *queens;
unsigned int local_solutions = 0;
MPI_Status status;
int rank;

// Profundidad a la que se trabajara con las iteraciones de trabajo repartido por el master.
int depth_col;

// Flag que indica que ya no hay mas trabajo.
int work_finished_flag = 0;

void slave(void);
void master(void);
double dwalltime();
void get_queens(int, int *, unsigned int *, int);
int get_next_work(int *, int);
void get_queens_master(int, int *, unsigned int *, int *, int);
void calculate_workload_depth();

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &P);

	N = atoi(argv[1]);
	queens = malloc(sizeof(int) * N);

	if (rank == MASTER_RANK)
		master();
	else
		slave();
	free(queens);
	MPI_Finalize();
}

void master()
{
	unsigned int total_solutions = 0;
	
	double timetick = dwalltime();

	if (P == 1) // Solo se encuentra el master ejecutando.
	{
		get_queens(0, queens, &total_solutions, N - 1);
		printf("N = %d\tP = %d\t# = %u\tTiempo = %.4f\n", N, P ,total_solutions, dwalltime() - timetick);
		if (valores_correctos[N] != total_solutions)
			printf("\nX\nX\nSolucion incorrecta.\nX\nX\n");
		return;
	}

	calculate_workload_depth();

	// Inicializo el vector que contendra la parcion de trabajo.
	int *q_next_work = malloc(sizeof(int) * depth_col);
	q_next_work[0] = 0;

	// Flag que indica si existen solicitudes de trabajo sin leer.
	int unread_msg; 

	// Bucle que itera siempre y cuando exista trabajo.
	while (get_next_work(q_next_work, depth_col - 1))
	{
		// Comprobar si es que existen solicitudes pendientes.
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &unread_msg, &status);
		if (!unread_msg)
		{
			// Como no hay slaves pidiendo trabajo, el master trabajo para no estar ocioso.
			// Copiar la parte inicial del trabajo para no alterar el historial de trabajo.
			memcpy(queens, q_next_work, sizeof(int) * depth_col);
			get_queens_master(depth_col, queens, &local_solutions, q_next_work, N - 1);
		}
		else
		{
			// Recibir solicitud y guardar en status el rank del slave correspondiente.
			MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
			// Enviar el trabajo.
			MPI_Send(q_next_work, depth_col, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
		}
	}

	// Al terminar todo el trabajo, se le informa a todos los slaves que concluyan con FINISH_TAG. 
	for (int r = 1; r < P; r++)
		MPI_Send(0, 0, MPI_INT, r, FINISH_TAG, MPI_COMM_WORLD);

	// Obtener el valor final de soluciones como suma de los valores locales de cada proceso.
	MPI_Reduce(&local_solutions, &total_solutions, 1, MPI_UNSIGNED, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);

	printf("N = %d\tP = %d\t# = %u\tTiempo = %.4f\n", N, P ,total_solutions, dwalltime() - timetick);
	if (valores_correctos[N] != total_solutions)
		printf("\nX\nX\nSolucion incorrecta.\nX\nX\n");

	free(q_next_work);
}

void slave()
{
	// Bucle infinito que concluye cuando recibo el mensaje correspondiente, con FINISH_TAG
	while (1)
	{
		// Enviar solicitud de trabajo al master
		MPI_Send(0, 0, MPI_INT, MASTER_RANK, WORK_TAG, MPI_COMM_WORLD);
		// Recibir respuesta
		MPI_Recv(queens, N, MPI_INT, MASTER_RANK, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		// Corroborar si es que hay que finalizar
		if (status.MPI_TAG == FINISH_TAG)
			break;
		// Obtener la cantidad de reinas envidas.
		MPI_Get_count(&status, MPI_INT, &depth_col);
		// Realizo el trabajo en base a la porcion inicial recibida.
		get_queens(depth_col, queens, &local_solutions, N - 1);
	}
	// Enviar el valor final
	MPI_Reduce(&local_solutions, &local_solutions, 1, MPI_UNSIGNED, MPI_SUM, MASTER_RANK, MPI_COMM_WORLD);
}

void get_queens(int col_start, int *queens, unsigned int *total_solutions, int col_final)
{
	int check;
	int j;
	int current_col = col_start;
	queens[col_start] = 0;
	while (1)
	{
		j = 0;
		check = 1;
		while (j < current_col && check)
		{
			if ((queens[j] == queens[current_col]) || (queens[j] == queens[current_col] - (j - current_col)) || (queens[j] == queens[current_col] + (j - current_col)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (current_col == col_final) // Era la ultima columna
				total_solutions[0]++;
			else // Sigo buscando
			{
				current_col++;
				queens[current_col] = 0;
				continue;
			}
		}
		//update indexes
		queens[current_col]++;
		if (queens[current_col] == N) //termino de analizar esta columna
		{
			if (current_col == col_start) //si era donde arranco, termine
				return;
			else
			{
				current_col--;
				queens[current_col]++;
				if (queens[current_col] == N)
				{
					if (current_col == col_start)
						return;
					else
					{
						current_col--;
						queens[current_col]++;
					}
				}
			}
		}
	}
}

void get_queens_master(int col_start, int *queens, unsigned int *total_solutions, int *q_next_work, int col_final)
{
	int check;
	int j;
	int current_col = col_start;
	int unread_msg;
	queens[col_start] = 0;
	while (1)
	{
		if (!work_finished_flag)
		{
			MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &unread_msg, &status);
			if (unread_msg)
			{
				if (get_next_work(q_next_work, depth_col - 1))
				{
					//recibir solicitudes
					MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, WORK_TAG, MPI_COMM_WORLD, &status);
					//mandarle mas trabajo
					MPI_Send(q_next_work, depth_col, MPI_INT, status.MPI_SOURCE, WORK_TAG, MPI_COMM_WORLD);
				}
			}
		}
		j = 0;
		check = 1;
		while (j < current_col && check)
		{
			if ((queens[j] == queens[current_col]) || (queens[j] == queens[current_col] - (j - current_col)) || (queens[j] == queens[current_col] + (j - current_col)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (current_col == col_final) // Era la ultima columna
				total_solutions[0]++;
			else // Sigo buscando
			{
				current_col++;
				queens[current_col] = 0;
				continue;
			}
		}
		//update indexes
		queens[current_col]++;
		if (queens[current_col] == N) //termino de analizar esta columna
		{
			if (current_col == col_start) //si era donde arranco, termine
				return;
			else
			{
				current_col--;
				queens[current_col]++;
				if (queens[current_col] == N)
				{
					if (current_col == col_start)
						return;
					else
					{
						current_col--;
						queens[current_col]++;
					}
				}
			}
		}
	}
}

int get_next_work(int *queens, int col_final)
{
	static int current_col = 0;
	static int update_flag = 0;
	int check;
	int j;

	while (!work_finished_flag)
	{
		//update indexes
		if (update_flag)
		{
			update_flag = 0;
			queens[current_col]++;
			if (queens[current_col] == N) //termino de analizar esta columna
			{
				if (current_col == 0) //termine
				{
					work_finished_flag = 1;
					return 0;
				}
				else
				{
					current_col--;
					queens[current_col]++;
					if (queens[current_col] == N)
					{
						if (current_col == 0) //termine
						{
							work_finished_flag = 1;
							return 0;
						}
						else
						{
							current_col--;
							queens[current_col]++;
						}
					}
				}
			}
		}
		//check
		j = 0;
		check = 1;
		while (j < current_col && check)
		{
			if ((queens[j] == queens[current_col]) || (queens[j] == queens[current_col] - (j - current_col)) || (queens[j] == queens[current_col] + (j - current_col)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (current_col == col_final) // Era la ultima columna
			{
				update_flag = 1; //quiero que a la vuelta, actualice
				return 1;	//encontro una opcion
			}
			else // Sigo buscando
			{
				current_col++;
				queens[current_col] = 0;
			}
		}
		else
			update_flag = 1;
	}
	return 0;
}

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
		workload = (N - 1) * (N - 2); //formula real para todo N > 4
	}
	else
	{
		depth_col = 2;
		workload = (N - 1) * (N - 2);
		int aux;
		while (workload < P && depth_col <= N)
		{
			aux = 0;
			depth_col++;
			get_queens(0, queens, &aux, (depth_col - 1));
			// printf("C=%d y work=%d\n", depth_col, aux);
			if (aux <= workload) // no tiene sentido seguir iterando
			{
				depth_col--; //tomar el caso anterior que tenia mas trabajo
				break;
			}
			workload = aux;
		}
	}
	// printf("Quedo en C=%d y work=%d\n", depth_col, workload);
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