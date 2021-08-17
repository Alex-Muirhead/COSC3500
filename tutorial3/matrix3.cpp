#include <iostream>
#include <chrono>
#include <vector>

#define n 2048
double A[n][n];
double B[n][n];
double C[n][n];

template <class T> 
T avg(std::vector<T> arr) {
    T sum = 0;
    for (int i = 0; i < arr.size(); i++) {
        sum += arr[i];
    }
    return sum / arr.size();
}

template <class T> 
T var(std::vector<T> arr) {
    if (arr.size() <= 1) {
        return 0;
    }
    T sum = 0;
    T mean = avg(arr);
    for (int i = 0; i < arr.size(); i++) {
        sum += (arr[i] - mean) * (arr[i] - mean);
    }
    return sum / (arr.size() - 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: ./matrix trials" << std::endl;
        return -1;
    }

    int trials = atoi(argv[1]);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = (double) rand() / (double) RAND_MAX;
            B[i][j] = (double) rand() / (double) RAND_MAX;
            C[i][j] = 0;
        }
    }
    int b = 32;
    std::vector<int> times;
    for (int i = 0; i < trials; i++) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int ii = 0; ii < n; ii += b) {
            for (int jj = 0; jj < n; jj += b) {
                for (int kk = 0; kk < n; kk += b) {
                    for (int i = 0; i < b; i++) {
                        for (int k = 0; k < b; k++) {
                            for (int j = 0; j < b; j++) {
                                C[i + ii][j + jj] += A[i + ii][k + kk] * B[k + kk][j + jj];
                            }
                        }
                    }
                }
            }
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
        times.push_back(duration);
    }
    std::cout << avg(times) << " " << var(times) << std::endl;
    return 0;
}