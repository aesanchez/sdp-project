#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>

unsigned long i,j;
unsigned long size;
double *a;

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
	double timetick;
	unsigned long N = atol(argv[1]);
	unsigned long print = atol(argv[2]);
	size = pow(2,N);

	a = (double *)malloc(sizeof(double) * size);

	//inicializamos
	for(i = 0; i < size ; i++)
	{
		a[i] = 1 + rand() % 10; //para que no sea cero
	}

	//imprimir original
	for(i = 0; i < size ; i++)
	{
		if(print) printf("%.2f\t",a[i]);
	}
	if(print) printf("\n");

	//operar
	timetick = dwalltime();
	for(i = 1; i <= N; i++)
	{
		for(j = 0; j < pow(2,N-i); j++)
		{
			a[j] = a[j*2] / a[j*2 + 1];
			if(print) printf("%.2f\t",a[j]);
		}
		if(print) printf("\n");
	}
	printf("Resultado final: %.5f\n", a[0]);
	printf("Tiempo: %f \n", dwalltime() - timetick);

	return 0;
}