#!/bin/bash
gcc -pthread pthread.c -o pthread
./pthread 512 2
./pthread 1024 2
./pthread 2048 2
./pthread 512 4
./pthread 1024 4
./pthread 2048 4