// COSC3500, Semester 2, 2021
// Assignment 2
// Main file - AVX version

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

// Sourced from https://stackoverflow.com/a/26905830
static inline double horizontal_add (__m256d a) {
   __m256d t1 = _mm256_hadd_pd(a,a);
   __m128d t2 = _mm256_extractf128_pd(t1,1);
   __m128d t3 = _mm_add_sd(_mm256_castpd256_pd128(t1),t2);
   return _mm_cvtsd_f64(t3);
}

// implementation of the matrix-vector multiply function
// void MatrixVectorMultiply(double* Y, const double* X)
// /* IN-COLUMN IMPLEMENTATION */
// {
//    // Assign space for variables
//    __m256d sum_chunk;
//    __m256d col_chunk;
//    __m256d mul_chunk;
//    __m256d vec_chunk;
//    // Set everything to zero first
//    for (int col = 0; col < N; ++col) Y[col] = 0.0;
//    for (int col = 0; col < N; ++col)
//    {
//       int row;
//       vec_chunk = _mm256_set1_pd(X[col]);
//       for (row = 0; row+4 <= N; row += 4)
//       {
//          sum_chunk = _mm256_loadu_pd(Y+row);
//          col_chunk = _mm256_from_ptr(M+col*N+row); // Use symmetry for spatial locality
//          mul_chunk = _mm256_mul_pd(col_chunk, vec_chunk);
//          // Acculuate elements
//          sum_chunk = _mm256_add_pd(sum_chunk, mul_chunk);
//          _mm256_storeu_pd(Y+row, sum_chunk);
//       }
//       // Clean up rows past last chunk of 4
//       for (; row < N; ++row) {
//          Y[row] += M[col*N+row] * X[col];
//       }
//    }
// }

// void MatrixVectorMultiply(double* Y, const double* X)
// /* IN-ROW IMPLEMENTATION */
// {
//    // Assign space for variables
//    __m256d sum_chunk;
//    __m256d col_chunk;
//    __m256d mul_chunk;
//    __m256d vec_chunk;
//    // Set everything to zero first
//    for (int col = 0; col < N; ++col) Y[col] = 0.0;
//    int row;
//    for (row = 0; row+4 <= N; row += 4)
//    {
//       sum_chunk = _mm256_loadu_pd(Y+row);
//       for (int col = 0; col < N; ++col)
//       {
//          vec_chunk = _mm256_set1_pd(X[col]);
//          col_chunk = _mm256_from_ptr(M+col*N+row); // Use symmetry for spatial locality
//          mul_chunk = _mm256_mul_pd(col_chunk, vec_chunk);
//          // Acculuate elements
//          sum_chunk = _mm256_add_pd(sum_chunk, mul_chunk);
//       }
//       _mm256_storeu_pd(Y+row, sum_chunk);
//       // Clean up rows past last chunk of 4
//    }
//    for (int end_row = row; end_row < N; ++end_row) {
//        for (int col = 0; col < N; ++col) {
//          Y[end_row] += M[end_row*N+col] * X[col];
//       }
//    }
// }

void MatrixVectorMultiply(double* Y, const double* X)
/* H-ADD IMPLEMENTATION */
{
   // Assign space for variables
   __m256d col_chunk;
   __m256d mul_chunk;
   __m256d vec_chunk;
   // Set everything to zero first
   int col;
   for (int row = 0; row <= N; ++row)
   {
      Y[row] = 0.0;
      for (col = 0; col+4 <= N; col += 4)
      {
         vec_chunk = _mm256_loadu_pd(X+col);
         col_chunk = _mm256_from_ptr(M+row*N+col);
         mul_chunk = _mm256_mul_pd(col_chunk, vec_chunk);
         // Acculuate elements
         Y[row] += horizontal_add(mul_chunk);
      }
      for (; col < N; ++col) {
         Y[row] += M[row*N+col] * X[col];
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
