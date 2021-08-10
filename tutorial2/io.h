/* COSC3500
   Example header file io.h to enable separate complation of the I/O functions
   Written by a previous tutor
   This comment added Ian McCulloch Semester 2, 2021
*/

#ifndef COSC3500_IO_H

#define N               100
#define T               100

int mat2hdf5 (double wdata[N][T], char filename[], char dataset[]);

#endif

