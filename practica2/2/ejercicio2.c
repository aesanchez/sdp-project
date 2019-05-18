#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define N 500000000
#define T 2

unsigned long long offset = 0;      //que thread soy
unsigned long long p = 4;	   //valor a buscar
unsigned long long ocurrencias = 0; //por que sibarita es tan rica
unsigned long long * vector;
unsigned long long * results; //resultados de cada thread

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
	int id = offset++;
	unsigned long long okurrr = 0; //ocurrencias locales
	// Each thread computes 1/nth of vector search
	for (unsigned long long i = id / T * N; i < (id + 1) / T * N; i++)
	{
		if (vector[i] == p)
			okurrr++;
	}
	results[id] = okurrr;

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{	
	results = malloc(sizeof(unsigned long long) * T);
	vector = malloc(sizeof(unsigned long long) * N);
	for (int i = 0; i < N; i++)
		vector[i] = rand() % 1000 + 1;

	// printf("Capacidad del arreglo: %d \n", N);
	// printf("Valor a buscar: %d \n", p);

	// printf("(");
	// for (unsigned long long i = 0; i < N - 1; i++)
	// {
	// 	printf("%d,", vector[i]);
	// }
	// printf("%d)\n", vector[N - 1]);

	// declaring four threads
	pthread_t threads[T];
	
	double timetick = dwalltime();

	// Creating threads, each evaluating its own part
	for (unsigned long long i = 0; i < T; i++)
		pthread_create(&threads[i], NULL, multi, NULL);

	// joining and waiting for all threads to complete
	for (unsigned long long i = 0; i < T; i++)
		pthread_join(threads[i], NULL);

	for (unsigned long long i = 0; i < T; i++)
		ocurrencias += results[i];
	printf("Tiempo: %f \n", dwalltime() - timetick);

	//printf("%lu\n", ocurrencias);
	return 0;
}