#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <omp.h>

using namespace std;
#define td 4

inline int64_t step(int64_t n) {
	int64_t output = 1; 	// make sure it terminates if there's some error
	if (n % 2) { output = 3 * n + 1; }
	else { output = n / 2; }
	return output;
}

int main(int argc, char* argv[]) {

	double t0 = omp_get_wtime();
	//omp_set_num_threads(td);
	// it should figure out how many threads you have automatically, but if not, use this

	int64_t start = 3;
	int64_t x;
	const bool check_evens = false;	
	const bool logging = false;

	// Set the chunk size for the thread distribution
	// in parallel for loop schedule below
	const int chunk_size = 1000;
	
	// Default value for upper bound
	int64_t N = 1000000;
	if (argc >= 2) { N = stoi(argv[1]); }
	
	//Find out the number of threads
	int num_td = td;
	#pragma omp parallel
	{
		num_td = omp_get_num_threads();
	}
	
	//Allocate some dynamic arrays to collect data from each thread
	int64_t* longest_start = new int64_t[num_td];
	int* longest_sequence = new int[num_td];
	int* intops = new int[num_td];
	int temp_len;
	int i;

	for (i = 0; i < num_td; i++) {
		longest_start[i] = 0;
		longest_sequence[i] = 0;
	}

	//Main calculation
	#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		int ops_count = 0;
		int seq_count = 0;
		int start_count = 0;

		#pragma omp for schedule(dynamic, chunk_size)
		for (int64_t j = start; j < N; j += (2 - check_evens)) {
			temp_len = 0;
			x = j;
			while (x != 1) { 
				x = step(x);
				temp_len++;
			}
			ops_count += temp_len;
			if (temp_len > seq_count) {
				seq_count = temp_len;
				start_count = j;
			}
		}

		longest_sequence[ID] = seq_count;
		longest_start[ID] = start_count;
		intops[ID] = ops_count;
	}

	int64_t output_start = 0;
	int output_seq = 0;
	int total_ops = 0;

	// Go through and collect the results from each thread's data
	for (i = 0; i < num_td; i++) {
		if ( longest_sequence[i] > output_seq ) {
			output_seq = longest_sequence[i];
			output_start = longest_start[i];
		}
		total_ops += intops[i];
	}

	double t1 = omp_get_wtime();
	cout << "Collatz sequence starting with " << output_start << " has " << output_seq << " terms\n";
	cout << "Computed " << total_ops << " steps.\n";
	cout << "Calculation took " << (t1 - t0) << " seconds\n";
	
	delete[] longest_start;
	delete[] longest_sequence;
	delete[] intops;

	if (logging) {
		x = output_start;
		int64_t* sequence = new int64_t[output_seq];
		int count = 0;

		// Calculate the sequence again
		while (x != 1) {
			sequence[count] = x;
			x = step(x);
			count++;
		}

		ofstream f;
		f.open("sequence.txt");
		for (i = 0; i < output_seq; i++) {
			f << sequence[i] << endl;
		}
		f.close();
		delete[] sequence;
	}
	return 0;
}
