#!/bin/bash
gcc -fopenmp openmp_junto.c -o openmp_junto
# ./openmp_junto 512 2
# ./openmp_junto 1024 2
# ./openmp_junto 2048 2
# ./openmp_junto 512 4
# ./openmp_junto 1024 4
./openmp_junto 2048 4