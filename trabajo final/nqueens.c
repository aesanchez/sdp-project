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

unsigned int N; //Tamanio vector
unsigned int *queens;
unsigned int total_solutions = 0;

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

int check(int last_index);

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
	//prueba agregar una reina en la columna index

	// printf("Agregando reina en la columna %d\n", index);
	// print_board();
	for (int i = 0; i < N; i++)
	{
		queens[index] = i;
		if (check(index))
		{
			if (index+1 == N)
			{
				// printf("SOLUCIOOOOOOOOOOON!!!\n");
				// print_board();
				// printf("\n");
				total_solutions++;
			}
			else
			{
				recursive_queens(index + 1);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	N = atoi(argv[1]);
	queens = malloc(sizeof(int) * N);
	for (int i = 0; i < N; i++)
		queens[i] = -2147483648;

	double timetick = dwalltime();

	recursive_queens(0);

	printf("Tiempo: %f | ", dwalltime() - timetick);
	printf("Soluciones con N = %d >> %d \n", N, total_solutions);

	return 0;
}

int check_viejo(int last_index)
{
	int check = 1;
	//una forma de chequear es agarrando la reina agregada en todas sus direcciones y fijarse si ya hay una reina o no (siempre para la izquierda)
	for (int i = 0; i < last_index; i++) //chequear lineas horizontales
	{
		if (queens[i] == queens[last_index])
			check = 0;
	}
	for (int i = 0; i < last_index; i++) //diagonales
	{
		if (queens[i] == queens[last_index] - (i - last_index) || queens[i] == queens[last_index] + (i - last_index))
			check = 0;
	}
	// if(!check){
	// 	printf("Solucion erronea (%d)\n", last_index);
	// 	print_board();
	// 	printf("\n\n");
	// }

	return check;
}
int check(int last_index)
{
	int check = 1;
	int i = 0;
	while (i < last_index && check)
	{
		if ((queens[i] == queens[last_index]) || (queens[i] == queens[last_index] - (i - last_index)) || (queens[i] == queens[last_index] + (i - last_index)))
			check = 0;
		i++;
	}
	return check;
}
