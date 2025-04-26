// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <complex>

// const int WIDTH = 800;
// const int HEIGHT = 600;
// const double ZOOM = 300.0;
// const int MAX_ITER = 1000;

// int mandelbrot(double x, double y) {
//     std::complex<double> z(0.0, 0.0);
//     std::complex<double> c(x, y);

//     int iter = 0;
//     while (std::norm(z) <= 4.0 && iter < MAX_ITER) {
//         z = z * z + c;
//         ++iter;
//     }
//     return iter;
// }

// int main() {
//     std::vector<std::vector<int>> result(HEIGHT, std::vector<int>(WIDTH));

//     for (int py = 0; py < HEIGHT; ++py) {
//         for (int px = 0; px < WIDTH; ++px) {
//             double x = px / ZOOM - 2.0;
//             double y = py / ZOOM - 1.5;
//             result[py][px] = mandelbrot(x, y);
//         }
//     }

//     // Optional: Write to file (e.g., CSV or PPM format)
//     std::ofstream out("mandelbrot_output.csv");
//     for (int py = 0; py < HEIGHT; ++py) {
//         for (int px = 0; px < WIDTH; ++px) {
//             out << result[py][px];
//             if (px != WIDTH - 1) out << ",";
//         }
//         out << "\n";
//     }
//     out.close();

//     std::cout << "Mandelbrot set computed and saved to mandelbrot_output.csv\n";
//     return 0;
// }


#include <iostream>
#include <vector>
#include <fstream>
#include <complex>
#include <future>   // for std::async and std::future
#include <algorithm> // for std::transform

const int WIDTH = 100;
const int HEIGHT = 3600;
const double ZOOM = 600.0;
const int MAX_ITER = 2000;

// Higher-order function: pixel coordinates -> number of iterations
int mandelbrot(double x, double y) {
    std::complex<double> z(0.0, 0.0);
    std::complex<double> c(x, y);

    int iter = 0;
    while (std::norm(z) <= 4.0 && iter < MAX_ITER) {
        z = z * z + c;
        ++iter;
    }
    return iter;
}

// Lazy computation of a single row (returns a future)
std::future<std::vector<int>> compute_row_async(int py) {
    return std::async(std::launch::async, [py]() {
        std::vector<int> row(WIDTH);
        std::transform(row.begin(), row.end(), row.begin(), [py](int, int px = 0) mutable {
            double x = px / ZOOM - 2.0;
            double y = py / ZOOM - 1.5;
            ++px;
            return mandelbrot(x, y);
        });
        return row;
    });
}

int main() {
    std::vector<std::future<std::vector<int>>> futures;

    // Parallelize computation across rows
    for (int py = 0; py < HEIGHT; ++py) {
        futures.push_back(compute_row_async(py));
    }

    // Collect the computed rows
    std::vector<std::vector<int>> result;
    result.reserve(HEIGHT);
    for (auto& future : futures) {
        result.push_back(future.get());  // Persistent structure
    }

    // Save result to CSV
    std::ofstream out("mandelbrot_output.csv");
    for (const auto& row : result) {
        for (size_t px = 0; px < row.size(); ++px) {
            out << row[px];
            if (px != row.size() - 1) out << ",";
        }
        out << "\n";
    }
    out.close();

    std::cout << "Parallel Mandelbrot set computed and saved to mandelbrot_output.csv\n";
    return 0;
}
