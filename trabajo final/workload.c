#include <mpi.h>
#include <unistd.h>
#include <stdio.h>

#define WORK_TAG 1
#define FINISH_TAG 2

void slave(void);
void master(void);

int rank;

int main(int argc, char *argv[])
{
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
		master();
	else
		slave();
	MPI_Finalize();
}

void master()
{
	int ntasks, rank, work;
	double result;
	MPI_Status status;
	MPI_Comm_size(MPI_COMM_WORLD, &ntasks);
	
	work = 0;
	for (rank = 1; rank < ntasks; ++rank) //tarea inicial
	{
		// work = nuevo trabajo para cada slave;
		MPI_Send(&work,1,MPI_INT,rank,WORK_TAG,MPI_COMM_WORLD);
	}
	work = 1;
	while (work < 10/* haya mas trabajo */)
	{
		//recibir resultados
		MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		//mandarle mas trabajo
		// work = nuevo trabajo para cada slave;       
		MPI_Send(&work, 1, MPI_INT, status.MPI_SOURCE,WORK_TAG, MPI_COMM_WORLD);
		work++;
	}

	//recibir trabajo de los restantes
	for (rank = 1; rank < ntasks; ++rank)
	{
		MPI_Recv(&result, 1, MPI_DOUBLE, MPI_ANY_SOURCE,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}

	// finalizar todos los slaves
	for (rank = 1; rank < ntasks; ++rank)
	{
		MPI_Send(0, 0, MPI_INT, rank, FINISH_TAG, MPI_COMM_WORLD);
	}
}

void slave()
{
	double result;
	int work;
	MPI_Status status;
	for (;;)
	{
		MPI_Recv(&work, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		if (status.MPI_TAG == FINISH_TAG)
			return;
		
		/* Start task */
		printf("Soy %d ejecutando el task %d\n", rank, work);
		usleep(rank*0.01); //simulation
		/* End task */

		MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
}
/*
mpirun -np 6 run.out
cada slave con diferentes tiempos de ejecucion
 
Soy 1 ejecutando el task 0
Soy 1 ejecutando el task 2
Soy 1 ejecutando el task 5
Soy 1 ejecutando el task 8

Soy 2 ejecutando el task 0
Soy 2 ejecutando el task 1
Soy 2 ejecutando el task 6

Soy 3 ejecutando el task 0
Soy 3 ejecutando el task 3
Soy 3 ejecutando el task 7
Soy 3 ejecutando el task 9

Soy 4 ejecutando el task 0

Soy 5 ejecutando el task 0
Soy 5 ejecutando el task 4
*/