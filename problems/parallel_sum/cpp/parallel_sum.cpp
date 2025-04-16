// sum_parallel.cpp
#include <iostream>
#include <omp.h>

int main() {
    const int size = 1e6;
    int* data = new int[size];
    for (int i = 0; i < size; ++i) data[i] = i + 1;

    long long total = 0;

    #pragma omp parallel for reduction(+:total)
    for (int i = 0; i < size; ++i) {
        total += data[i];
    }

    std::cout << "Sum is " << total << std::endl;
    delete[] data;
    return 0;
}
