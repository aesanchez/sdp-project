#!/bin/bash
mpicc nqueens_mpi.c -o mpi
for N in {5..14}
do
   mpirun -np 1 mpi $N
done
for N in {5..14}
do
   mpirun -np 4 mpi $N
done
