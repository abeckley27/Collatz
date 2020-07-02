#!/bin/bash
g++ collatz.cpp -o collatz -O2 -fopenmp
mpic++ mpi_collatz.cpp -o mpicollatz -O2
