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
double* Mdevice = nullptr;

double* Ydevice = nullptr;
double* Xdevice = nullptr;

int N = 0;
int Blocks = 0;

const int BLOCKSIZE = 256;

void checkError(cudaError_t e)
{
   if (e != cudaSuccess)
   {
      std::cerr << "CUDA error: " << int(e) << " : " << cudaGetErrorString(e) << '\n';
      abort();
   }
}

// 59 us @ N=100, 558 us @ N=1000, 5658 us @ N=10_000
// __global__
// void MatVecMulKernel(int N, double* Y, const double* X, const double* M)
// {
//    // Each thread calculates a single value of Y.
//    int row = blockIdx.x*BLOCKSIZE + threadIdx.x;
//    double Yvalue = 0.0;
//    __shared__ double Xsub[BLOCKSIZE];
//    for (int subIdx = 0; subIdx < gridDim.x; ++subIdx)
//    {
//       int precol = threadIdx.x + subIdx*BLOCKSIZE;
//       if (precol < N)
//          Xsub[threadIdx.x] = X[precol];
//       __syncthreads();
//       for (int subcol = 0; subcol < BLOCKSIZE; ++subcol)
//       {
//          int col = subcol + subIdx*BLOCKSIZE;
//          if (col >= N || row >= N) break;
//          Yvalue += M[col*N+row] * Xsub[subcol];
//       }
//       __syncthreads();
//    }
//    if (row < N)
//       Y[row] = Yvalue;
// }

// 50 us @ N=100, 254 us @ N=1000, 4036 us @ N=10_000
__global__
void MatVecMulKernel(int N, double* Y, const double* X, const double* M)
{
   // Each thread calculates a single value of Y.
   int i = blockIdx.x*BLOCKSIZE + threadIdx.x;
   if (i >= N) return;
   double Yvalue = 0.0;
   for (int j = 0; j < N; ++j)
   {
      Yvalue += M[j*N+i] * X[j];
   }
   Y[i] = Yvalue;
}

// implementation of the matrix-vector multiply function
void MatrixVectorMultiply(double* Y, const double* X)
{
   // Total threads = Blocks * ThreadsPerBlock >= N
   // At least one thread per row of matrix, avoids data-race at Y
   checkError(cudaMemcpy(Xdevice, X, N*sizeof(double), cudaMemcpyHostToDevice));
   MatVecMulKernel<<<Blocks, BLOCKSIZE>>>(N, Ydevice, Xdevice, Mdevice);
   checkError(cudaMemcpy(Y, Ydevice, N*sizeof(double), cudaMemcpyDeviceToHost));
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

   Blocks = (N+BLOCKSIZE-1)/BLOCKSIZE;

   // Allocate memory for the matrix
   M = static_cast<double*>(malloc(N*N*sizeof(double)));
   checkError(cudaMalloc(&Mdevice, N*N*sizeof(double)));

   // Allocate CUDA memory space for vectors
   checkError(cudaMalloc(&Ydevice, N*sizeof(double)));
   checkError(cudaMalloc(&Xdevice, N*sizeof(double)));

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
   checkError(cudaMemcpy(Mdevice, M, N*N*sizeof(double), cudaMemcpyHostToDevice));
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
   checkError(cudaFree(Xdevice));
   checkError(cudaFree(Ydevice));
   checkError(cudaFree(Mdevice));
   free(M);
}
