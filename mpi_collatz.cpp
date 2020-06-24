#include <iostream>
#include <cstdint>
#include <string>
#include <mpi.h>

using namespace std;

inline int64_t step(int64_t n) {
	int64_t output = 1; // make it more likely to terminate if there's some error :P
	if (n % 2) { output = 3 * n + 1; }
	else { output = n / 2; }
	return output;
}

int main(int argc, char* argv[]) {
    
    double t0 = MPI_Wtime();
    int64_t N = stoi(argv[1]);

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int* longest_seq = new int[size];
    int64_t* longest_start = new int64_t[size];
    for (int i = 0; i < size; i++) {longest_seq[i] = 0;}

    int64_t start = 3 + 2 * rank;
    int64_t x;
	int temp_len;
	int i;
    int proc_step = 2 * size;

	for (int64_t j = start; j < N; j += proc_step) {
		temp_len = 0;
		x = j;
		while (x != 1) { 
			x = step(x);
			temp_len++;
		}
		if (temp_len > longest_seq[rank]) {
			longest_seq[rank] = temp_len;
			longest_start[rank] = j;
		}
	}

    if ( rank != 0 ) {
        MPI_Send(&longest_seq[rank], 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(&longest_start[rank], 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    if (rank == 0) {
        int k;
        int64_t output_num = longest_start[0];
        int output_len = longest_seq[0];

        for (k = 1; k < size; k++ ) {
            MPI_Recv(&longest_seq[k], 1, MPI_INT, k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&longest_start[k], 1, MPI_INT, k, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        cout << "n_proc = " << size << endl;

        for (k = 1; k < size; k++) {
            if (longest_seq[k] > output_len) {
                output_len = longest_seq[k];
                output_num = longest_start[k];
            }
        }

        cout << "Collatz sequence starting with " << output_num;
        cout << " has length " << output_len << endl;
    }

    MPI_Finalize();
    double t1 = MPI_Wtime();
    cout << "Time: " << (t1 - t0) << endl;

    return 0;
}