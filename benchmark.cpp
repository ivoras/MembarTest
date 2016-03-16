#include <atomic>
#include <iostream>
#include <iomanip>

#include <sys/time.h>

double now() {
  struct timeval tv;

  gettimeofday(&tv, NULL);
  return tv.tv_sec + (double)(tv.tv_usec) / 1e6;
}

#define DIMENSION 512

// toggle what "int" means
#define int std::atomic<int>

int first[DIMENSION][DIMENSION], second[DIMENSION][DIMENSION], multiply[DIMENSION][DIMENSION];

void matrixmult() {
  int m, n, p, q, c, d, k, sum;
  sum = 0;
  m = n = p = q = DIMENSION;
  for (c = 0; c < m; c++) {
    for (d = 0; d < q; d++) {
      for (k = 0; k < p; k++) {
        sum = sum + first[c][k]*second[k][d];
      }
      multiply[c][d] = (long)sum;
      sum = 0;
    }
  }
}

#undef int

int main(int argc, char **argv) {
  const int n_iterations = 10;
  double t1, t2;

  std::cout << "Single matrix in-memory size: " << sizeof(first) << " bytes (" << sizeof(first) / 1024 << " KiB)." << std::endl;

  for (int i = 0; i < n_iterations; i++)
    matrixmult();

  std::cout << "Done warmup." << std::endl;

  t1 = now();

  for (long i = 0; i < n_iterations; i++)
    matrixmult();

  t2 = now();

  std::cout << "A single multiplication took " << std::fixed << std::setprecision(4) << (t2 - t1) / n_iterations << " seconds." << std::endl;
}
