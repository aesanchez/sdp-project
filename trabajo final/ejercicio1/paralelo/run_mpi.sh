#!/bin/bash
mpicc nqueens_mpi.c -o mpi
for N in {1..15}
do
   mpirun -np 4 mpi $N
done