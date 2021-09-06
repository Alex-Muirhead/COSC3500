#include <iostream>
#include <chrono>

using namespace std;

void checkError(cudaError_t e)
{
   if (e != cudaSuccess)
   {
      std::cerr << "CUDA error: " << int(e) << " : " << cudaGetErrorString(e) << '\n';
      abort();
   }
}

// code that will run on the GPU, but can call it from the CPU
__global__
void add(int n, double* x, double const* y)
{
   int index = threadIdx.x;
   int stride = blockDim.x;     // blockDim is the number of threads in a block
   for (int i = index; i < n; i += stride)
   {
      x[i] = x[i] + y[i];
   }
}

int main()
{
   int N = 1<<20; // pow(2,20) = 1,048,576

   // allocate memory on the host
   double* x = new double[N];
   double* y = new double[N];

   // initialize arrays
   for (int i = 0; i < N; i++)
   {
      x[i] = 1.0;
      y[i] = 2.0;
   }

   // allocate memory on the device
   double* xDevice;
   double* yDevice;
   checkError(cudaMalloc(&xDevice, N*sizeof(double)));
   checkError(cudaMalloc(&yDevice, N*sizeof(double)));

   // copy memory from host to device
   checkError(cudaMemcpy(xDevice, x, N*sizeof(double), cudaMemcpyHostToDevice));
   checkError(cudaMemcpy(yDevice, y, N*sizeof(double), cudaMemcpyHostToDevice));

   auto t1 = std::chrono::high_resolution_clock::now();
   // Invoke the CUDA kernel with add<<<NumberOfBlocks, NumberOfThreadsPerBlock>>>
   add<<<1, 1>>>(N, xDevice, yDevice); // asynchronous
   checkError(cudaDeviceSynchronize());
   auto t2 = std::chrono::high_resolution_clock::now();

   // copy memory from device back to host
   checkError(cudaMemcpy(x, xDevice, N*sizeof(double), cudaMemcpyDeviceToHost));

   // check the result
   for (int i = 0; i < N; ++i)
   {
      if (x[i] != 3.0)
      {
         std::cerr << "error at array index " << i << " value " << x[i] << " expected 3.0\n";
         std::abort();
      }
   }

   auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
   std::cout << "Time = " << duration << " us\n";

   // clean up
   checkError(cudaFree(xDevice));
   checkError(cudaFree(yDevice));
   delete[] x;
   delete[] y;
}
