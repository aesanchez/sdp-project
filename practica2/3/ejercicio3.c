#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define VECTORCAP 100000000
#define MAXTHREADS 1

//que thread soy
int offset = 0;

//por que sibarita es tan rica
int minimo = 99999;
int maximo = -1;

int vector[VECTORCAP];

//resultados de cada thread
int minimos[MAXTHREADS];
int maximos[MAXTHREADS];

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

void *multi(void *arg)
{
	int core = offset++;

	//ocurrencias locales
	int min = 999999;
	int max = -1;

	// Each thread computes 1/nth of vector search
	for (int i = core * VECTORCAP / MAXTHREADS; i < (core + 1) * VECTORCAP / MAXTHREADS; i++)
	{
		if (vector[i] < min)
			min = vector[i];
		if (vector[i] > max)
			max = vector[i];
	}
	minimos[core] = min;
	maximos[core] = max;

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

	for (int i = 0; i < VECTORCAP; i++)
	{
		vector[i] = rand() % 50 + 1;
	}
	// printf("Capacidad del arreglo: %d \n", VECTORCAP);

	// printf("(");
	// for (int i = 0; i < VECTORCAP - 1; i++)
	// {

	// 	printf("%d,", vector[i]);
	// }
	// printf("%d)\n", vector[VECTORCAP - 1]);

	// declaring four threads
	pthread_t threads[MAXTHREADS];

	double timetick = dwalltime();

	// Creating threads, each evaluating its own part
	for (int i = 0; i < MAXTHREADS; i++)
	{
		//     int* p;
		//(void*)(p)
		pthread_create(&threads[i], NULL, multi, NULL);
	}

	// joining and waiting for all threads to complete
	for (int i = 0; i < MAXTHREADS; i++)
		pthread_join(threads[i], NULL);

	for (int i = 0; i < MAXTHREADS; i++)
	{
		if (minimos[i] < minimo)
			minimo = minimos[i];
	}

	for (int i = 0; i < MAXTHREADS; i++)
	{
		if (maximos[i] > maximo)
			maximo = maximos[i];
	}

	printf("Tiempo: %f \n", dwalltime() - timetick);

	printf("Minimo: %d\n", minimo);
	printf("Maximo: %d\n", maximo);

	return 0;
}