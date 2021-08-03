#include "hdf5.h"
#include <stdio.h>
#include <stdlib.h>

#define FILE            "particles.h5"
#define DATASET         "DS1"
#define N               100
#define T               100
#define DECAY_CHANCE    0.03

int mat2hdf5 (double wdata[N][T]) {
    hid_t       file, space, dset;          /* Handles */
    herr_t      status;
    hsize_t     dims[2] = {N, T};

    /*
     * Create a new file using the default properties.
     */
    file = H5Fcreate (FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Create dataspace.  Setting maximum size to NULL sets the maximum
     * size to be the current size.
     */
    space = H5Screate_simple (2, dims, NULL);

    /*
     * Create the dataset and write the floating point data to it.  In
     * this example we will save the data as 64 bit little endian IEEE
     * floating point numbers, regardless of the native type.  The HDF5
     * library automatically converts between different floating point
     * types.
     */
    dset = H5Dcreate (file, DATASET, H5T_IEEE_F64LE, space, H5P_DEFAULT,
                H5P_DEFAULT, H5P_DEFAULT);
    status = H5Dwrite (dset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                wdata[0]);

    /*
     * Close and release resources.
     */
    status = H5Dclose (dset);
    status = H5Sclose (space);
    status = H5Fclose (file);

    return 0;
}

int main (void) {
    double time_series[N][T] = {0};
    float random_float;
    float decay_rate = 0.4;

    for (int i=0; i<N; i++) {
        for (int t=0; t<T; t++) {
            if (t==0) {
                time_series[i][t] = 1; // initial radiation
            } else {
                random_float = (float)rand()/(float)(RAND_MAX); //random float from 0 to 1
                if (random_float<DECAY_CHANCE) { // less than DECAY_CHANCE, particle decay
                    time_series[i][t] = decay_rate*time_series[i][t-1];
                } else { // no decay
                    time_series[i][t] = time_series[i][t-1];
                }
            }
        }
    }

    mat2hdf5(time_series);
    return 0;
}
