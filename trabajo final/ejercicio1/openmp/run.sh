#!/bin/bash
gcc -fopenmp openmp.c -o openmp
# ./openmp 512 2
# ./openmp 1024 2
# ./openmp 2048 2
# ./openmp 512 4
# ./openmp 1024 4
./openmp 2048 4