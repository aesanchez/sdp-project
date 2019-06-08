/*Paralelizar un algoritmo paralelo que ordene un vector de N elementos por mezcla.
Ejecutar con 2 y 4 Threads.
*/
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

unsigned int N;    //Tamanio vector
unsigned int T;    //cantidad de threads
unsigned int Tlog; //log de T
unsigned int *A;
sem_t * semaphores;

double dwalltime()
{
	double sec;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	sec = tv.tv_sec + tv.tv_usec / 1000000.0;
	return sec;
}

typedef struct str
{
	int id;
	int offset;
	int cantidad;
} thread_args;

void crear_matrices(void);
void imprimir(unsigned int *);
void sort_arrays(int start, int end, int partitions, unsigned int *A_aux);
void merge(int start, int end, unsigned int *A_aux);

void merge(int start, int end, unsigned int *A_aux)
{
	// printf("Merge Start %d End %d\n", start, end);
	int size = end - start + 1;
	if (size > 2)
	{
		merge(start, start + size / 2 - 1, A_aux); //izq
		merge(start + size / 2, end, A_aux);       //der
		//al volver aca, voy a tener dos tramos ordenados internamente
		sort_arrays(start, end, 2, A_aux);
	}
	else
	{
		if (size == 1)
			return;
		//size=2
		//ordenar
		if (A[start] > A[end])
		{
			//swap
			int aux = A[start];
			A[start] = A[end];
			A[end] = aux;
		}
	}
	// imprimir(A);
}
void sort_arrays(int start, int end, int partitions, unsigned int *A_aux)
{
	int i = 0;
	int size = end - start + 1;
	int partition_size = size / partitions;
	int *partitions_index = malloc(sizeof(int) * partitions);

	// printf("Sort_arrays= Start: %d End: %d Partitions: %d\n", start, end, partitions);
	// for (i = start; i <= end; i++)
	// {
	// 	printf("%2.0u  ", A[i]);
	// }
	// printf("\n");

	for (i = 0; i < partitions; i++)
	{
		partitions_index[i] = start + partition_size * i;
	}
	int min, min_j;
	for (i = 0; i < size; i++)
	{
		min = 9999;
		// printf("i=%d\n",i);
		//buscar el minimo en todos los arreglos
		for (int j = 0; j < partitions; j++)
		{
			// printf("\tj=%d\n",j);
			// printf("\tmin=%d a[]=%d\n", min, A[partitions_index[j]]);
			// printf("\tp_d[j]=%d ?? %d\n", partitions_index[j] , partition_size * (j + 1));
			if (partitions_index[j] != start + partition_size * (j + 1))
			{
				if (A[partitions_index[j]] < min)
				{
					min = A[partitions_index[j]];
					min_j = j;
				}
			}
		}
		A_aux[i] = min;
		partitions_index[min_j]++;
	}
	// pasar todo al array original
	for (i = 0; i < size; i++)
	{
		A[i + start] = A_aux[i];
	}
	// for (i = start; i <= end; i++)
	// {
	// 	printf("%2.0u  ", A[i]);
	// }
	// printf("\n");
}

void *thread_function(void *arg)
{
	thread_args my_args = *(thread_args *)arg;
	// printf("Id: %d / Start: %d / End:%d\n", my_args.id, my_args.offset, my_args.offset + my_args.cantidad - 1);
	//sort
	unsigned int *A_aux = malloc(sizeof(unsigned int) * my_args.cantidad);
	merge(my_args.offset, my_args.offset + my_args.cantidad - 1, A_aux);
	for (int i = 1; i <= Tlog; i++)
	{
		if(my_args.id % (1<<i) == 0){
			
			sem_wait(&semaphores[my_args.id/(1<<i)]);
			A_aux = malloc(sizeof(unsigned int) * my_args.cantidad * (1<<i));
			sort_arrays(my_args.offset, my_args.offset+(my_args.cantidad*(1<<i))-1, 2, A_aux);
		}
		else{
			sem_post(&semaphores[(my_args.id - (1<<(i-1)))/(1<<i)]);
			break;
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	N = atoi(argv[1]);
	N = 1 << N;
	Tlog = atoi(argv[2]);
	T = 1 << Tlog;
	semaphores = malloc(sizeof(sem_t)*T/2);
	for(int i = 0; i < T/2; i++)
		sem_init(&semaphores[i], 0, 0);
	crear_matrices();
	// imprimir(A);

	pthread_t *threads;
	threads = malloc(sizeof(pthread_t) * T);
	thread_args *args_array = malloc(sizeof(thread_args) * T);
	double timetick;
	timetick = dwalltime();
	for (int t = 0; t < T; t++)
	{
		args_array[t].id = t;
		args_array[t].cantidad = N / T;
		args_array[t].offset = N / T * t;
		pthread_create(&threads[t], NULL, &thread_function, (void *)&args_array[t]);
	}
	for (int t = 0; t < T; t++)
	{
		pthread_join(threads[t], NULL);
	}
	// Al terminar tengo los 2 cachos del vector ordenados
	unsigned int * A_aux = malloc(sizeof(unsigned int) * N);
	sort_arrays(0, N-1, 2, A_aux);

	printf("Tiempo con N = %d T = %d: %f \n", N, T, dwalltime() - timetick);
	// imprimir(A);

	return 0;
}

void crear_matrices()
{
	A = malloc(sizeof(unsigned int) * N);
	for (int i = 0; i < N; i++)
	{
		A[i] = rand() % 100;
	}
}
void imprimir(unsigned int *m)
{
	for (int i = 0; i < N; i++)
	{
		printf("%2.0u  ", m[i]);
	}
	printf("\n");
}