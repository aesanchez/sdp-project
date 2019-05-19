//Ejercicio 2
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

int main(int argc, char *argv[])
{
	double x, scale;
	int i;
	int numThreads = atoi(argv[2]);
	int N = atoi(argv[1]);
	omp_set_num_threads(numThreads);
	scale = 2.78;

	x = 0.0;
	for (i = 1; i <= N; i++)
	{
		x = sqrt(i * scale) + 3 * x;
	}
	printf("\nResultado secuencial original: %f \n", x);

	x = 0.0;
	//vemos que no funca porque los valores de X depende de valores anteriores.
#pragma omp parallel for
	for (i = 1; i <= N; i++)
	{
		x = sqrt(i * scale) + 3 * x;
	}

	printf("\nResultado paralelo original: %f \n", x);
/*======================================================================*/
	/*
	analizando la funcion podemos decir que es:
	x(i) = raiz(i*scale)+3.x(i-1)
	desenrrollando...
	x(0) = 0
	x(1) = raiz(1*scale) + 3 . x(0) = raiz(1*scale)
	x(2) = raiz(2*scale) + 3 . x(1) = raiz(2*scale) + 3 . raiz(1*scale)
	x(3) = raiz(3*scale) + 3 . x(2) = raiz(3*scale) + 3 . (raiz(2*scale) + 3 . raiz(1*scale))
	     = raiz(3*scale) + 3 . raiz(2*scale) + 9 . raiz(1*scale)
	x(4) = raiz(4*scale) + 3 . x(3) = raiz(4*scale) + 3 . (raiz(3*scale) + 3 . raiz(2*scale) + 9 . raiz(1*scale))
	     = raiz(4*scale) + 3 . raiz(3*scale) + 9 . raiz(2*scale) + 27 . raiz(1*scale)
	Por lo tanto
	x(N) = sumatoria de i=1..N(pow(3, N-i) * sqrt(i * scale))
	*/

	x = 0.0;
	for (i = 1; i <= N; i++)
	{
		x += pow(3, N-i) * sqrt(i * scale);
	}
	printf("\nResultado secuencial mejorado: %f \n", x);

	x = 0.0;
	//reduction(...) realiza una operacion matematica sobre las variables privadas de todos los hilos al finalizar.
#pragma omp parallel for reduction(+:x)
	for (i = 1; i <= N; i++)
	{
		x += pow(3, N-i) * sqrt(i * scale);
	}

	printf("\nResultado paralelo mejorado: %f \n", x);

	return (0);
}
