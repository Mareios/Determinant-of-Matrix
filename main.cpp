//
//  main.cpp
//  Coursework_MarekTomastik
//
//  Created by Marek Tomaštík on 15.01.17.
//  Copyright © 2017 Marek Tomaštík. All rights reserved.
//

#include <iostream>
#include <math.h>
#include <random>
#include <chrono>
#include <thread>


template <typename TimePoint>
std::chrono::milliseconds to_ms(TimePoint tp) {
	return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

int get_random_int() {
	thread_local std::random_device mt;
	thread_local std::mt19937 gen(mt());
	std::uniform_int_distribution<> dis(0, 10);
	
	return dis(gen);
}


void withoutThreads(const int sizeMatrix) {
	std::cout << "Without threads\n";
	
	/*  --------  Input  --------  */
	
	const int N = sizeMatrix; // size of square matrix (velikost čtvercové matice)
	
	double a[N][N]; // matrix whose values will be generated in main() function (matice jejíž prvky se vygenerují ve funkci main())
	
	/*double a[N][N] = { // already defined matrix (matice již definovaná)
	 {2, 3, 5},
	 {1, 1, 2},
	 {3, 1, 1}
	 // determinant is 3
	 };*/
	
	double determinant = 0;
	
	/*  ----------------  */
	
	
	// initialization of matrix if is not initialized manually (inicializace matice, pokud jsme ji neinicializovali ručně)
	//auto initM = [N, a]() {
		for (int x = 0; x < N; x++) {
			for (int y = 0; y < N; y++) {
				a[x][y] = get_random_int();
			}
		}
	//};
	
	
	// start of time measurement (začátek měření času)
	auto start = std::chrono::high_resolution_clock::now();
	
	
	// listing of matrix (kontrolní výpis zadané matice)
	std::cout << "\nGiven matrix: \n";
	for (int u = 0; u < N; u++) {
		for (int v = 0; v < N; v++) {
			std::cout << a[u][v] << ", ";
		}
		std::cout << " \n";
	}
	
	
	// gaussian elimination
	int max;
	
	for (int i = 0; i < N; i++) {
		max = i;
		
		for (int j = i + 1; j < N; j++) {
			if (fabs(a[j][i]) > fabs(a[max][i])) {
				max = j;
			}
		}
		
		if (i != max) {
			//std::cout << "Change if row " << i << " and " << max << "\n";
			
			// change of rows
			for (int k = i; k < N; k++) {
				double doc = a[i][k];
				a[i][k] = -a[max][k];
				a[max][k] = doc;
			}
		}
		
		// listing of eliminated matrix (kontrolní výpis upravené matice)
		//std::cout << "\nEliminated matrix: \n";
		//vypisM();
		
		determinant = 1.0;
		
		if (a[i][i] != 0) {
			for (int j = i + 1; j < N; j++) { // j = row
				// multiplication and sum of rows (násobení a součet řádků)
				for (int k = N-1; k >= i; k--) { // k = column
					a[j][k] -= a[i][k] * a[j][i] / a[i][i];
				}
			}
		} else {
			determinant = 0.0;
		}
		
	}
	//std::cout << "------------------\n";
	
	
	// calculation of determinant from diagonal (výpočet determinantu z diagonály)
	for (int u = 0; u < N; u++) {
		determinant *= a[u][u];
	}
	
	// end of time measurement
	auto end = std::chrono::high_resolution_clock::now();
	
	std::cout << "\nDeterminant is " << determinant << "\n";
	std::cout << "Duration " << to_ms(end - start).count() << " ms.\n";
}


/*  ----------------  *//*  ----------------  *//*  ----------------  *//*  ----------------  *//*  ----------------  *//*  ----------------  */

int num_threads = std::thread::hardware_concurrency();

void withThreads(const int sizeMatrix) {
	
	std::cout << "With threads\n";
	
	/*  --------  Input  --------  */
	
	const int N = sizeMatrix; // size of square matrix (velikost čtvercové matice)
	
	double **a = new double*[N]; // matrix whose values will be generated in main() function (matice jejíž prvky se vygenerují ve funkci main())
	for (int i = 0; i < N; ++i)
		a[i] = new double[N];
	
	double determinant = 1.0;
	
	/*  ----------------  */
	
	// initialization of matrix if is not initialized manually (inicializace matice, pokud jsme ji neinicializovali ručně)
	for (int x = 0; x < N; x++) {
		for (int y = 0; y < N; y++) {
			a[x][y] = get_random_int();
		}
	}
	
	// listing of matrix
	std::cout << "\nGiven matrix: \n";
	for (int u = 0; u < N; u++) {
		for (int v = 0; v < N; v++) {
			std::cout << a[u][v] << ", ";
		}
		std::cout << " \n";
	}
	
	// start of time measurement
	auto start = std::chrono::high_resolution_clock::now();
	
	int max;
	
	std::vector<std::thread> threads;
	threads.reserve(num_threads);
	
	for (int i = 0; i < N; i++) {
		max = i;
		
		for (int j = i + 1; j < N; j++) {
			if (fabs(a[j][i]) > fabs(a[max][i])) {
				max = j;
			}
		}
		
		if (i != max) {
			//std::cout << "Changing row " << i << " and " << max << "\n";
			
			// change of rows
			for (int k = i; k < N; k++) {
				double doc = a[i][k];
				a[i][k] = -a[max][k];
				a[max][k] = doc;
			}
		}
		
		/*  --------  THREADS  --------  */
		
		auto f = [i, N](int begin, int end, double **matrix) {
			for (int j = begin; j < end; j++) { // j = row
				// multiplication and sum of rows
				for (int k = i+1; k < N; k++) { // k = column
					matrix[j][k] -= matrix[i][k] * matrix[j][i] / matrix[i][i];
				}
			}
		};
		
		/*  ----------------  */
		
		int max_workload = ceil(static_cast<float>(N)/num_threads);
		int current_offset = i+1;
		int rows_left = N-current_offset;
		
		if (a[i][i] != 0) {
			if (num_threads < 2) {
				f(current_offset, rows_left, a);
			} else {
				threads.clear();
				
				// start of threads
				for (int p = 0; p < num_threads; p++) {
					threads.emplace_back(f, current_offset, current_offset+fmin(rows_left, max_workload), a);
					current_offset += max_workload;
					rows_left -= max_workload;
				}
				
				// joinování vláken
				for (int p = 0; p < num_threads; p++) {
					threads[p].join();
				}
				
			}
		} else {
			determinant = 0.0;
		}
	}
	
	// calculation of determinant from diagonal (výpočet determinantu z diagonály)
	for (int u = 0; u < N; u++) {
		determinant *= a[u][u];
	}
	
	// end of time measurement
	auto end = std::chrono::high_resolution_clock::now();
	
	std::cout << "\nDeterminant is " << determinant << "\n";
	std::cout << "Duration " << to_ms(end - start).count() << " ms.\n";
	
	for (int i = 0; i < N; ++i) {
		delete [] a[i];
	}
	delete [] a;
}



int main(int argc, const char * argv[]) {
	
	if (argc == 3 && std::string(argv[1]) == "normal" && isdigit(*argv[2])) {
		withoutThreads(atoi(argv[2]));
	} else if (argc == 3 && std::string(argv[1]) == "threads" && isdigit(*argv[2])) {
		withThreads(atoi(argv[2]));
	} else {
		std::cout << "Wrong arguments (Špatně zadané argumenty)\n";
	}
	
}
