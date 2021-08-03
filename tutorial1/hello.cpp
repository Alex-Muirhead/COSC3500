/*******************************************************************************
About: This is a first program in C++ to get started. It will print a
Hello World message to the screen and then exit
Compile: g++ helloworld.cpp -Wall -o out.exe
Input: None
Output: Prints out a hello world message using the std::cout command
which sends the output to the screen
**********************80*character*limit*good*for*a2ps*************************/

#include <iostream>

int main()
{
   std::cout << "Hello World!"<<'\n'; // Print something
   //exit(0);
   return 0;
}
