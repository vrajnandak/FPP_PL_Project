#include <iostream>
#include <vector>
#include <fstream>
#include <complex>
#include <future>
#include <thread>
#include <functional>
#include <chrono> 

const int WIDTH = 1600;
const int HEIGHT = 1200;
const double ZOOM = 300.0;
const int MAX_ITER = 1000;

// Compute the Mandelbrot iteration count for a point
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

// Compute a single row of the Mandelbrot set
std::vector<int> compute_row(int py) {
    std::vector<int> row(WIDTH);
    for (int px = 0; px < WIDTH; ++px) {
        double x = static_cast<double>(px) / ZOOM - 2.0;
        double y = static_cast<double>(py) / ZOOM - 1.5;
        row[px] = mandelbrot(x, y);
    }
    return row;
}

// int main() {
//     // Step 1: Lazily create lambdas (not executed yet)
//     std::vector<std::function<std::vector<int>()>> lazy_tasks;
//     for (int py = 0; py < HEIGHT; ++py) {
//         lazy_tasks.emplace_back([py]() {
//             return compute_row(py);
//         });
//     }

//     // Step 2: Now execute them in parallel using async
//     std::vector<std::future<std::vector<int>>> futures;
//     for (auto& task : lazy_tasks) {
//         futures.push_back(std::async(std::launch::async, task));
//     }

//     // Step 3: Gather results
//     std::vector<std::vector<int>> result(HEIGHT);
//     for (int py = 0; py < HEIGHT; ++py) {
//         result[py] = futures[py].get();
//     }

//     // Step 4: Write to CSV
//     std::ofstream out("mandelbrot_output.csv");
//     for (const auto& row : result) {
//         for (size_t i = 0; i < row.size(); ++i) {
//             out << row[i] << (i + 1 == row.size() ? "\n" : ",");
//         }
//     }
//     out.close();

//     std::cout << "Parallel Mandelbrot set saved to mandelbrot_output.csv (with lazy evaluation)\n";
//     return 0;
// }

int main() {
    std::vector<std::function<std::vector<int>()>> lazy_tasks;
    for (int py = 0; py < HEIGHT; ++py) {
        lazy_tasks.emplace_back([py]() {
            return compute_row(py);
        });
    }

    std::vector<std::future<std::vector<int>>> futures;

    auto start = std::chrono::high_resolution_clock::now(); // Start timing

    for (auto& task : lazy_tasks) {
        futures.push_back(std::async(std::launch::async, task));
    }

    std::vector<std::vector<int>> result(HEIGHT);
    for (int py = 0; py < HEIGHT; ++py) {
        result[py] = futures[py].get();
    }

    auto end = std::chrono::high_resolution_clock::now(); // End timing
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Computation Time: " << elapsed.count() << " seconds\n";

    // Separate CSV writing
    std::ofstream out("mandelbrot_output.csv");
    for (const auto& row : result) {
        for (size_t i = 0; i < row.size(); ++i) {
            out << row[i] << (i + 1 == row.size() ? "\n" : ",");
        }
    }
    out.close();

    std::cout << "Output saved to mandelbrot_output.csv\n";
    return 0;
}



// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <complex>
// #include <future>      // for std::async, std::future
// #include <thread>      // for std::thread::hardware_concurrency

// const int WIDTH = 800;
// const int HEIGHT = 600;
// const double ZOOM = 300.0;
// const int MAX_ITER = 1000;

// // Compute the Mandelbrot iteration count for a point
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

// // Compute a single row of the Mandelbrot set
// std::vector<int> compute_row(int py) {
//     std::vector<int> row(WIDTH);
//     for (int px = 0; px < WIDTH; ++px) {
//         double x = static_cast<double>(px) / ZOOM - 2.0;
//         double y = static_cast<double>(py) / ZOOM - 1.5;
//         row[px] = mandelbrot(x, y);
//     }
//     return row;
// }

// int main() {
//     std::vector<std::future<std::vector<int>>> futures;

//     // Launch tasks in parallel (one per row)
//     for (int py = 0; py < HEIGHT; ++py) {
//         futures.push_back(std::async(std::launch::async, compute_row, py));
//     }

//     // Collect results
//     std::vector<std::vector<int>> result(HEIGHT);
//     for (int py = 0; py < HEIGHT; ++py) {
//         result[py] = futures[py].get();
//     }

//     // Write to CSV
//     std::ofstream out("mandelbrot_output.csv");
//     for (const auto& row : result) {
//         for (size_t i = 0; i < row.size(); ++i) {
//             out << row[i] << (i + 1 == row.size() ? "\n" : ",");
//         }
//     }
//     out.close();

//     std::cout << "Parallel Mandelbrot set saved to mandelbrot_output.csv\n";
//     return 0;
// }



// correct but faster

// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <complex>
// #include <future>      // for std::async, std::future
// #include <thread>      // for std::thread::hardware_concurrency

// const int WIDTH = 800;
// const int HEIGHT = 600;
// const double ZOOM = 300.0;
// const int MAX_ITER = 1000;

// // Compute the Mandelbrot iteration count for a point
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

// // Compute a single row of the Mandelbrot set
// std::vector<int> compute_row(int py) {
//     std::vector<int> row(WIDTH);
//     for (int px = 0; px < WIDTH; ++px) {
//         double x = px / ZOOM - 2.0;
//         double y = py / ZOOM - 1.5;
//         row[px] = mandelbrot(x, y);
//     }
//     return row;
// }

// int main() {
//     // A vector of futures for each row (like Task.async)
//     std::vector<std::future<std::vector<int>>> futures;

//     // Launch tasks in parallel (parallel loop over rows)
//     for (int py = 0; py < HEIGHT; ++py) {
//         futures.push_back(std::async(std::launch::async, compute_row, py));
//     }

//     // Collect results (like Task.await)
//     std::vector<std::vector<int>> result(HEIGHT);
//     for (int py = 0; py < HEIGHT; ++py) {
//         result[py] = futures[py].get();
//     }

//     // Write to CSV
//     std::ofstream out("mandelbrot_output.csv");
//     for (const auto& row : result) {
//         for (size_t i = 0; i < row.size(); ++i) {
//             out << row[i] << (i + 1 == row.size() ? "\n" : ",");
//         }
//     }
//     out.close();

//     std::cout << "Parallel Mandelbrot set saved to mandelbrot_output.csv\n";
//     return 0;
// }








//    INCORRECT CODE
// #include <iostream>
// #include <vector>
// #include <fstream>
// #include <complex>
// #include <future>   // for std::async and std::future
// #include <algorithm> // for std::transform

// const int WIDTH = 100;
// const int HEIGHT = 3600;
// const double ZOOM = 600.0;
// const int MAX_ITER = 2000;

// // Higher-order function: pixel coordinates -> number of iterations
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

// // Lazy computation of a single row (returns a future)
// std::future<std::vector<int>> compute_row_async(int py) {
//     return std::async(std::launch::async, [py]() {
//         std::vector<int> row(WIDTH);
//         std::transform(row.begin(), row.end(), row.begin(), [py](int, int px = 0) mutable {
//             double x = px / ZOOM - 2.0;
//             double y = py / ZOOM - 1.5;
//             ++px;
//             return mandelbrot(x, y);
//         });
//         return row;
//     });
// }

// int main() {
//     std::vector<std::future<std::vector<int>>> futures;

//     // Parallelize computation across rows
//     for (int py = 0; py < HEIGHT; ++py) {
//         futures.push_back(compute_row_async(py));
//     }

//     // Collect the computed rows
//     std::vector<std::vector<int>> result;
//     result.reserve(HEIGHT);
//     for (auto& future : futures) {
//         result.push_back(future.get());  // Persistent structure
//     }

//     // Save result to CSV
//     std::ofstream out("mandelbrot_output.csv");
//     for (const auto& row : result) {
//         for (size_t px = 0; px < row.size(); ++px) {
//             out << row[px];
//             if (px != row.size() - 1) out << ",";
//         }
//         out << "\n";
//     }
//     out.close();

//     std::cout << "Parallel Mandelbrot set computed and saved to mandelbrot_output.csv\n";
//     return 0;
// }
