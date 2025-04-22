#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <chrono>

const double PI = acos(-1);
using Complex = std::complex<double>;
using namespace std;

inline Complex omega(int k, int n) {
    double angle = -2.0 * PI * k / n;
    return Complex(cos(angle), sin(angle));
}

void fft(vector<Complex>& a, vector<Complex>& out, int start, int n, int step) {
    if (n == 1) {
        out[start / step] = a[start];
        return;
    }

    int half = n / 2;
    vector<Complex> even(half), odd(half);

    if (n > 2048) {
        #pragma omp task shared(a)
        fft(a, even, start, half, 2 * step);

        #pragma omp task shared(a)
        fft(a, odd, start + step, half, 2 * step);

        #pragma omp taskwait
    } 
    
    else 
    {
        fft(a, even, start, half, 2 * step);
        fft(a, odd, start + step, half, 2 * step);
    }

    for (int k = 0; k < half; ++k) {
        Complex w = omega(k, n) * odd[k];
        out[k] = even[k] + w;
        out[k + half] = even[k] - w;
    }
}

int main() {
    int n = 1 << 20; // 2^20 = 1,048,576 points
    vector<Complex> input(n, 1); // All values set to 1
    vector<Complex> output(n);

    omp_set_num_threads(4); // Set number of threads

    auto start_time = chrono::high_resolution_clock::now();

    #pragma omp parallel
    {
        #pragma omp single
        {
            fft(input, output, 0, n, 1);
        }
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;

    cout << "Parallel FFT done in: " << duration.count() << " seconds" << endl;

    return 0;
}
