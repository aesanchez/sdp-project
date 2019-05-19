#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <math.h>

#define MAXTHREADS 1
#define N 25
unsigned long long VECTORCAP;
//que thread soy
int offset = 0;

//por que sibarita es tan rica
long double promedio = 0;

unsigned long long *vector;

//resultados de cada thread
unsigned long long * parciales;

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
	parciales[core] = 0;

	for (int i = core * VECTORCAP / MAXTHREADS; i < (core + 1) * VECTORCAP / MAXTHREADS; i++)
		parciales[core] += vector[i];

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	VECTORCAP = pow(2, N);
	vector = malloc(sizeof(unsigned long long) * VECTORCAP);
	parciales = malloc(sizeof(unsigned long long) * MAXTHREADS);
	for (int i = 0; i < VECTORCAP; i++)
	{
		vector[i] = rand() % 10 + 1;
	}

	// printf("(");
	// for (int i = 0; i < VECTORCAP - 1; i++)
	// {

	// 	printf("%lld,", vector[i]);
	// }
	// printf("%lld)\n", vector[VECTORCAP - 1]);

	// declaring four threads
	pthread_t threads[MAXTHREADS];

	double timetick = dwalltime();

	// Creating threads, each evaluating its own part
	for (int i = 0; i < MAXTHREADS; i++)
	{
		pthread_create(&threads[i], NULL, multi, NULL);
	}
	// joining and waiting for all threads to complete
	for (int i = 0; i < MAXTHREADS; i++)
		pthread_join(threads[i], NULL);

	for (int i = 0; i < MAXTHREADS; i++)
	{
		promedio += parciales[i];
	}

	// promedio /= VECTORCAP;

	printf("Tiempo: %f \n", dwalltime() - timetick);

	printf("Promedio: %Lf\n", promedio);

	return 0;
}