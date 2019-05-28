#!/bin/bash
gcc -fopenmp nqueens_parallel.c -o parallel
for N in {1..15}
do
   ./parallel $N 4
done
