# Collatz
Brute force checking of Collatz sequences or the 3n+1 problem. The program finds the starting number which has the longest Collatz sequence, up to a given upper bound.
There are two implementations of the code; both are multi-threaded. collatz.cpp uses the shared-memory library OpenMP, and mpi_collatz.cpp uses the distributed-memory library MPI.
