#!/bin/bash
gcc -fopenmp openmp_separado.c -o openmp_separado
./openmp_separado 512 2
./openmp_separado 1024 2
./openmp_separado 2048 2
./openmp_separado 512 4
./openmp_separado 1024 4
./openmp_separado 2048 4