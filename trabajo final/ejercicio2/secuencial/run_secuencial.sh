#!/bin/bash
gcc nqueens_secuencial.c -o secuencial
for N in {5..15}
do
   ./secuencial $N
done