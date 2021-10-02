// COSC3500, Semester 2, 2021
// Assignment 2
// Main file - serial version

#include "eigensolver.h"
#include "randutil.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <immintrin.h>

#define ALIGN 64
#define _mm256_from_ptr(ptr) *((__m256d*)(ptr))

// global variables to store the matrix

double* M = nullptr;
int N = 0;

// implementation of the matrix-vector multiply function
void MatrixVectorMultiply(double* Y, const double* X)
{
   __m256d vec_chunk = _mm256_set1_pd(X[0]);
   int j;
   for (j = 0; j < N; j += 4)  // j is the column
   {
      __m256d row_chunk = _mm256_from_ptr(M+j);
      __m256d mul_chunk = _mm256_mul_pd(vec_chunk, row_chunk);
      _mm256_storeu_pd(Y+j, mul_chunk);
   }
   for (; j < N; ++j)
   {
      Y[0] += M[j] * X[j];
   }
   for (int i = 1; i < N; i++)  // i is the row
   {
      __m256d vec_chunk = _mm256_set1_pd(X[i]);
      for (j = 0; j < N; j += 4)  // j is the column
      {
         __m256d out_chunk = _mm256_loadu_pd(Y+j);
         __m256d row_chunk = _mm256_from_ptr(M+i*N+j);
         __m256d mul_chunk = _mm256_mul_pd(row_chunk, vec_chunk);
         // out_chunk = _mm256_fmadd_pd(vec_chunk, row_chunk, out_chunk);
         // Acculuate elements
         out_chunk = _mm256_add_pd(out_chunk, mul_chunk);
         _mm256_storeu_pd(Y+j, out_chunk);
      }
      for (; j < N; ++j)
      {
         Y[i] += M[i*N+j] * X[j];
      }
   }
}

int main(int argc, char** argv)
{
   // get the current time, for benchmarking
   auto StartTime = std::chrono::high_resolution_clock::now();

   // get the input size from the command line
   if (argc < 2)
   {
      std::cerr << "expected: matrix size <N>\n";
      return 1;
   }
   N = std::stoi(argv[1]);

   // Allocate memory for the matrix
   M = static_cast<double*>(aligned_alloc(ALIGN, N*N*sizeof(double)));

   if (M == nullptr) {
      std::cerr << "failed to allocate aligned memory\n";
      return 1;
   }

   // seed the random number generator to a known state
   randutil::seed(4);  // The standard random number.  https://xkcd.com/221/

   // Initialize the matrix.  This is a matrix from a Gaussian Orthogonal Ensemble.
   // The matrix is symmetric.
   // The diagonal entries are gaussian distributed with variance 2.
   // The off-diagonal entries are gaussian distributed with variance 1.
   for (int i = 0; i < N; ++i)
   {
      M[i*N+i] = std::sqrt(2.0) * randutil::randn();
      for (int j = i+1; j < N; ++j)
      {
         M[i*N + j] = M[j*N + i] = randutil::randn();
      }
   }
   auto FinishInitialization = std::chrono::high_resolution_clock::now();

   // Call the eigensolver
   EigensolverInfo Info = eigenvalues_arpack(N, 100);

   auto FinishTime = std::chrono::high_resolution_clock::now();

   auto InitializationTime = std::chrono::duration_cast<std::chrono::microseconds>(FinishInitialization - StartTime);
   auto TotalTime = std::chrono::duration_cast<std::chrono::microseconds>(FinishTime - StartTime);

   std::cout << "Obtained " << Info.Eigenvalues.size() << " eigenvalues.\n";
   std::cout << "The largest eigenvalue is: " << std::setw(16) << std::setprecision(12) << Info.Eigenvalues.back() << '\n';
   std::cout << "Total time:                             " << std::setw(12) << TotalTime.count() << " us\n";
   std::cout << "Time spent in initialization:           " << std::setw(12) << InitializationTime.count() << " us\n";
   std::cout << "Time spent in eigensolver:              " << std::setw(12) << Info.TimeInEigensolver.count() << " us\n";
   std::cout << "   Of which the multiply function used: " << std::setw(12) << Info.TimeInMultiply.count() << " us\n";
   std::cout << "   And the eigensolver library used:    " << std::setw(12) << (Info.TimeInEigensolver - Info.TimeInMultiply).count() << " us\n";
   std::cout << "Total serial (initialization + solver): " << std::setw(12) << (TotalTime - Info.TimeInMultiply).count() << " us\n";
   std::cout << "Number of matrix-vector multiplies:     " << std::setw(12) << Info.NumMultiplies << '\n';
   std::cout << "Time per matrix-vector multiplication:  " << std::setw(12) << (Info.TimeInMultiply / Info.NumMultiplies).count() << " us\n";

   // free memory
   free(M);
}