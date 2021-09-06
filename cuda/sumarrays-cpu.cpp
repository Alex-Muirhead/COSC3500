#include <iostream>
#include <chrono>
#include <cstdlib>

void add(int n, double* x, double const* y)
{
   for (int i = 0; i < n; ++i)
   {
      x[i] = x[i] + y[i];
   }
}

int main()
{
   int N = 1<<20; // pow(2,20) = 1,048,576

   // allocate memory
   double* x = new double[N];
   double* y = new double[N];

   // initialize arrays
   for (int i = 0; i < N; i++)
   {
      x[i] = 1.0;
      y[i] = 2.0;
   }

   // do the computation
   auto t1 = std::chrono::high_resolution_clock::now();
   add(N, x, y);
   auto t2 = std::chrono::high_resolution_clock::now();

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
   delete[] x;
   delete[] y;
}
