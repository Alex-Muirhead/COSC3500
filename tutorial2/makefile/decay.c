#include "io.h"
#include <stdio.h>
#include <stdlib.h>

#define FILENAME        "particles.h5"
#define DATASET         "DS1"
#define DECAY_CHANCE    0.04



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

    mat2hdf5(time_series, FILENAME, DATASET);
    return 0;
}
