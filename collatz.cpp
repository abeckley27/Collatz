#include <iostream>
#include <cstdint>
#include <string>
#include <omp.h>

using namespace std;

#define td 4

inline int64_t step(int64_t n) {
	int64_t output = 1; // make sure it terminates if there's some error
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
	int64_t N = 1000000;
	if (argc >= 1) { N = stoi(argv[1]); }

	int64_t longest_start[td];
	int longest_sequence[td];
	int temp_len;
	int i;

	for (i = 0; i < td; i++) {
		longest_start[i] = 0;
		longest_sequence[i] = 0;
	}

	#pragma omp parallel
	{
		int ID = omp_get_thread_num();
		#pragma omp for schedule(dynamic, 64)
		for (int64_t j = start; j < N; j += 2) {
			temp_len = 0;
			x = j;
			while (x != 1) { 
				x = step(x);
				temp_len++;
			}
			if (temp_len > longest_sequence[ID]) {
				longest_sequence[ID] = temp_len;
				longest_start[ID] = j;
			}
		}
	}

	int64_t output_start = 0;
	int output_seq = 0;
	for (i = 0; i < td; i++) {
		if ( longest_sequence[i] > output_seq ) {
			output_seq = longest_sequence[i];
			output_start = longest_start[i];
		}

	}

	double t1 = omp_get_wtime();
	cout << "Collatz sequence starting with " << output_start << " has " << output_seq << " terms\n";
	cout << "Calculation took " << (t1 - t0) << " seconds\n";

	return 0;
}
