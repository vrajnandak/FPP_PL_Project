#include <iostream>
#include <vector>
#include <fstream>
#include <complex>
#include <thread>
#include <chrono>

// Image and Mandelbrot parameters
const int WIDTH = 1600;
const int HEIGHT = 1200;
const double ZOOM = 300.0;
const int MAX_ITER = 1000;
const int NUM_THREADS = 8; // Number of worker threads for parallelism

// Mandelbrot iteration function: returns number of iterations before escape
int mandelbrot(double x, double y) {
    std::complex<double> z(0.0, 0.0);     // Initial z value
    std::complex<double> c(x, y);         // Constant c = current point
    int iter = 0;

    // Iterate until |z|^2 > 4 or max iterations reached
    while (std::norm(z) <= 4.0 && iter < MAX_ITER) {
        z = z * z + c;  // Mandelbrot formula: z = z^2 + c
        ++iter;
    }
    return iter; // Return iteration count (used for color mapping)
}

// Thread worker: computes rows [start, end) and stores in 'result'
void compute_rows(int start, int end, std::vector<std::vector<int>>& result) {
    for (int py = start; py < end; ++py) {
        std::vector<int> row(WIDTH); // Create row buffer
        for (int px = 0; px < WIDTH; ++px) {
            // Convert pixel to complex plane coordinates
            double x = static_cast<double>(px) / ZOOM - 2.0;
            double y = static_cast<double>(py) / ZOOM - 1.5;
            row[px] = mandelbrot(x, y); // Compute iteration count
        }
        result[py] = std::move(row); // Store row in result vector
    }
}

int main() {
    std::vector<std::vector<int>> result(HEIGHT); // Stores the full image data
    std::vector<std::thread> threads;             // Container for thread objects

    auto start_time = std::chrono::high_resolution_clock::now(); // Start timing

    // Divide work among threads (each thread processes a block of rows)
    int rows_per_thread = HEIGHT / NUM_THREADS;
    for (int i = 0; i < NUM_THREADS; ++i) {
        int start = i * rows_per_thread;
        int end = (i == NUM_THREADS - 1) ? HEIGHT : start + rows_per_thread;
        threads.emplace_back(compute_rows, start, end, std::ref(result)); // Launch thread
    }

    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now(); // End timing
    std::chrono::duration<double> elapsed = end_time - start_time;
    std::cout << "Computation Time: " << elapsed.count() << " seconds\n";

    // Write the output to a CSV file
    std::ofstream out("mandelbrot_output.csv");
    for (const auto& row : result) {
        for (size_t i = 0; i < row.size(); ++i) {
            out << row[i] << (i + 1 == row.size() ? "\n" : ","); // Format CSV
        }
    }
    out.close();
    std::cout << "Output saved to mandelbrot_output.csv\n";

    return 0;
}
