#include <cstdio>
#include <thread>

using namespace std;

int x = 0;
int y = 0;

void thread1()
{
   y = x+1;
   printf("y=%d\n", y);
}

void thread2()
{
   x = y+1;
   printf("x=%d\n", x);
}

int main()
{
   thread t1(thread1);
   thread t2(thread2);

   t1.join();
   t2.join();
}
