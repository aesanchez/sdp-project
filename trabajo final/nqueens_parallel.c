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
#include <omp.h>

unsigned int N; //Tamanio vector
unsigned int T;
unsigned int total = 0;

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void recursive_queens(int index, unsigned int * queens, unsigned int * total_solutions)
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
			if (index + 1 == N) 	// Era la ultima reina
			{
				total_solutions[0] += 1;
			}	
			else			// Sigo buscando
			{
				queens[index] = i;
				recursive_queens(index + 1, queens, total_solutions);
			}
				
		}
	}
}

int main(int argc, char *argv[])
{
	N = atoi(argv[1]);
	T = atoi(argv[2]);
	omp_set_num_threads(T);

	double timetick = dwalltime();

#pragma omp parallel shared(total)
{
	unsigned int *queens = malloc(sizeof(int) * N);
	unsigned int total_solutions = 0;
	#pragma omp for schedule(dynamic, 1)
	for (int i = 0; i < N; i++)
	{
		queens[0] = i;
		recursive_queens(1, queens, &total_solutions);
	}
	printf("Soluciones encontradas por ID:%d >> %d \n", omp_get_thread_num(), total_solutions);
	total += total_solutions;
}
	printf("N = %d\t| Soluciones = %d\t| Tiempo = %.4f\n", N, total, dwalltime() - timetick);\

	return 0;
}