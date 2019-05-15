#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

//Dimension por defecto de las matrices
int N = 100;

//Para calcular tiempo
double dwalltime()
{
	double sec;
	struct timeval tv;
	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

int main(int argc, char *argv[])
{
	double *A, *Atranspuesta, *result;
	int i, j, k;
	int check = 1;
	double timetick;

	//Controla los argumentos al programa
	if ((argc != 2) || ((N = atoi(argv[1])) <= 0))
	{
		printf("\nUsar: %s n\n  n: Dimension de la matriz (nxn X nxn)\n", argv[0]);
		exit(1);
	}

	//Aloca memoria para las matrices
	A = (double *)malloc(sizeof(double) * N * N);
	Atranspuesta = (double *)malloc(sizeof(double) * N * N);
	result = (double *)malloc(sizeof(double) * N * N);
	double aux = 0;
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			A[i * N + j] = aux++; //x filas
		}
	}
	//Realiza la multiplicacion
	timetick = dwalltime();
	//Transponer
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			Atranspuesta[i * N + j] = A[i + j * N];
		}
	}
	
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			result[i * N + j] = 0; //x filas
			for (k = 0; k < N; k++)
			{
				result[i * N + j] += A[i * N + k] * Atranspuesta[k + j * N]; 
				//Atranspuesta se almacena por filas pero se trata como por columnas. Termina calculando A*A.
			}
		}
	}

	printf("Tiempo en segundos %f\n", dwalltime() - timetick);

	// printf("Matriz A\n");
	// for (i = 0; i < N; i++){
	// 	for (j = 0; j < N; j++){
	// 		printf("%.0f\t", A[i * N + j]);
	// 	}
	// 	printf("\n");
	// }
	// printf("Matriz A transpuesta\n");
	// for (i = 0; i < N; i++){
	// 	for (j = 0; j < N; j++){
	// 		printf("%.0f\t", Atranspuesta[i * N + j]);
	// 	}
	// 	printf("\n");
	// }
	// printf("Matriz result\n");
	// for (i = 0; i < N; i++){
	// 	for (j = 0; j < N; j++){
	// 		printf("%.0f\t", result[i * N + j]);
	// 	}
	// 	printf("\n");
	// }

	free(A);
	free(Atranspuesta);
	free(result);
	return (0);
}
