#!/bin/bash
gcc nqueens_optimizado.c
for N in {1..15}
do
   ./a.out $N
done