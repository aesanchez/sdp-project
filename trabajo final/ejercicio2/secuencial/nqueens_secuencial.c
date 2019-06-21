/**
 * El juego de las N-Reinas consiste en ubicar sobre un tablero de ajedrez N
 * reinas sin que estas se amenacen entre ellas.
 * Una reina amenaza a aquellas reinas que se encuentren en su misma fila,
 * columna o diagonal.
 * La solución al problema de las N-Reinas consiste en encontrar todas las
 * posibles soluciones para un tablero de tamaño NxN.
 */
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include "valores_correctos.h"

#define ITERATIVE 1
#define RECURSIVE 2

/**
 * Flag de ejecucion iterativo o paralelo. 
 * En la catedra se recomendo no utilizar algoritmos recursivos.
 */
#define MODE ITERATIVE

unsigned int N;
unsigned int *queens;
unsigned int total_solutions = 0;
int check, j;

double dwalltime();
void print_board();
void get_queens_recursive(int);
void get_queens();

int main(int argc, char *argv[])
{
	double timetick;
	N = atoi(argv[1]);
	queens = malloc(sizeof(int) * N);

	timetick = dwalltime();

	switch (MODE)
	{
	case ITERATIVE:
		get_queens();
		break;

	case RECURSIVE:
		get_queens_recursive(0);
		break;
	}

	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n", N, total_solutions, dwalltime() - timetick);
	if (valores_correctos[N] != total_solutions)
		printf("\nX\nX\nSolucion incorrecta.\nX\nX\n");

	free(queens);
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
 * Impresion del tablero para debugeo.
 */
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

/**
 * Funcion recursiva para el calculo de soluciones totales.
 * @param current_col es el indice de la columna actual de ejecucion.
 */
void get_queens_recursive(int current_col)
{
	for (int i = 0; i < N; i++)
	{
		queens[current_col] = i;

		// Inicio de checkeo
		check = 1;
		j = 0;
		while (j < current_col && check)
		{
			if ((queens[j] == queens[current_col]) || (queens[j] == queens[current_col] - (j - current_col)) || (queens[j] == queens[current_col] + (j - current_col)))
				check = 0;
			j++;
		}
		// Fin de checkeo

		if (check)
		{
			if (current_col + 1 == N)
				total_solutions++; // Era la ultima reina, sumo una solucion.
			else
				get_queens_recursive(current_col + 1); // Continuo buscando.
		}
	}
}

/**
 * Funcion recursiva para el calculo de soluciones totales.
 */
void get_queens()
{
	int current_col = 0;
	queens[0] = 0;
	while (1)
	{
		// Inicio de checkeo
		j = 0;
		check = 1;
		while (j < current_col && check)
		{
			if ((queens[j] == queens[current_col]) || (queens[j] == queens[current_col] - (j - current_col)) || (queens[j] == queens[current_col] + (j - current_col)))
				check = 0;
			j++;
		}
		// Fin de checkeo

		if (check)
		{
			if (current_col == N - 1) // Era la ultima reina, sumo una solucion.
				total_solutions++;
			else // Continuo buscando.
			{
				current_col++;
				queens[current_col] = 0;
				continue;
			}
		}

		// Update indexes
		queens[current_col]++;
		if (queens[current_col] == N) // Se termino de analizar esta columna
		{
			if (current_col == 0) // Si vuelve a punto de comienzo, se termina.
				return;
			else
			{
				current_col--; // Volver a la columna anterior.
				queens[current_col]++;
				if (queens[current_col] == N) // Se termino de analizar esta columna
				{
					if (current_col == 0) // Si vuelve a punto de comienzo, se termina.
						return;
					else
					{
						current_col--; // Volver a la columna anterior.
						queens[current_col]++;
					}
				}
			}
		}
	}
}