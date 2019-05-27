#include <mpi.h>

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

	for (rank = 1; rank < ntasks; ++rank) //tarea inicial
	{
		// work = nuevo trabajo para cada slave;
		MPI_Send(&work,1,MPI_INT,rank,WORK_TAG,MPI_COMM_WORLD);
	}

	while (/* haya mas trabajo */)
	{
		//recibir resultados
		MPI_Recv(&result,1,MPI_DOUBLE,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		//mandarle mas trabajo
		// work = nuevo trabajo para cada slave;       
		MPI_Send(&work, 1, MPI_INT, status.MPI_SOURCE,WORK_TAG, MPI_COMM_WORLD);
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

		/* do the work */

		MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
	}
}