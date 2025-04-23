#include <iostream>
#include <vector>
#include <fstream>
#include <complex>

const int WIDTH = 800;
const int HEIGHT = 600;
const double ZOOM = 300.0;
const int MAX_ITER = 1000;

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

int main() {
    std::vector<std::vector<int>> result(HEIGHT, std::vector<int>(WIDTH));

    for (int py = 0; py < HEIGHT; ++py) {
        for (int px = 0; px < WIDTH; ++px) {
            double x = px / ZOOM - 2.0;
            double y = py / ZOOM - 1.5;
            result[py][px] = mandelbrot(x, y);
        }
    }

    // Optional: Write to file (e.g., CSV or PPM format)
    std::ofstream out("mandelbrot_output.csv");
    for (int py = 0; py < HEIGHT; ++py) {
        for (int px = 0; px < WIDTH; ++px) {
            out << result[py][px];
            if (px != WIDTH - 1) out << ",";
        }
        out << "\n";
    }
    out.close();

    std::cout << "Mandelbrot set computed and saved to mandelbrot_output.csv\n";
    return 0;
}
