/*
El juego de las N-Reinas consiste en ubicar sobre un tablero de ajedrez N
reinas sin que estas se amenacen entre ellas.
Una reina amenaza a aquellas reinas que se encuentren en su misma fila,
columna o diagonal.
La solución al problema de las N-Reinas consiste en encontrar todas las
posibles soluciones para un tablero de tamaño NxN.
*/
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "valores_correctos.h"

unsigned int N; //Tamanio vector
unsigned int *queens;
unsigned int total_solutions = 0;
int check, j;
double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void print_board()
{
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (queens[j] == i)
				printf("Q ");
			else
				printf("X ");
		}
		printf("\n");
	}
	printf("\n");
}

void recursive_queens(int index)
{
	for (int i = 0; i < N; i++)
	{
		queens[index] = i;
		// Inicio de checkeo
		check = 1;
		j = 0;
		while (j < index && check)
		{
			if ((queens[j] == queens[index]) || (queens[j] == queens[index] - (j - index)) || (queens[j] == queens[index] + (j - index)))
				check = 0;
			j++;
		}
		// Fin de checkeo
		if (check)
		{
			if (index + 1 == N) 	// Era la ultima reina
				total_solutions++;
			else 			// Sigo buscando
				recursive_queens(index + 1);
		}
	}
}
void get_queens()
{
	int col = 0;
	queens[0] = 0;
	while (1)
	{
		j = 0;
		check = 1;
		while (j < col && check)
		{
			if ((queens[j] == queens[col]) || (queens[j] == queens[col] - (j - col)) || (queens[j] == queens[col] + (j - col)))
				check = 0;
			j++;
		}
		if (check)
		{
			if (col == N-1) // Era la ultima columna
				total_solutions++;
			else // Sigo buscando
			{
				col++;
				queens[col] = 0;
				continue;
			}
		}
		//update index
		queens[col]++;
		if (queens[col] == N) //termino de analizar esta columna
		{
			if (col == 0) //si era donde arranco, termine
				return;
			else
			{
				col--;
				queens[col]++;
				if (queens[col] == N)
				{
					if (col == 0)
						return;
					else
					{
						col--;
						queens[col]++;
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	double timetick;
	N = atoi(argv[1]);
	queens = malloc(sizeof(int) * N);

	printf("Solucion recursiva\n");
	timetick = dwalltime();
	recursive_queens(0);	
	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n", N, total_solutions, dwalltime() - timetick);
	if (valores_correctos[N] != total_solutions)
		printf("Solucion incorrecta.\nX\nX\nX\nX\nX\n");

	printf("Solucion no-recursiva\n");
	total_solutions = 0;
	timetick = dwalltime();
	get_queens();
	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n", N, total_solutions, dwalltime() - timetick);
	if (valores_correctos[N] != total_solutions)
		printf("Solucion incorrecta.\nX\nX\nX\nX\nX\n");
	
	free(queens);
	return 0;
}