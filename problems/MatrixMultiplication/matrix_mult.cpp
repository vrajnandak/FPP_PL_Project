#include <iostream>
#include <vector>
#include <random>
#include <chrono>

// Type alias for cleaner code
using Matrix = std::vector<std::vector<int>>;

// Function to generate a random matrix
Matrix create_random_matrix(int rows, int cols) {
    std::mt19937 rng(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(0, 99);

    Matrix mat(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            mat[i][j] = dist(rng);
    return mat;
}

// Regular matrix multiplication
Matrix matrix_mult(const Matrix& a, const Matrix& b) {
    int rows_a = a.size();
    int cols_a = a[0].size();
    int cols_b = b[0].size();

    Matrix result(rows_a, std::vector<int>(cols_b, 0));
    for (int i = 0; i < rows_a; ++i) {
        for (int j = 0; j < cols_b; ++j) {
            for (int k = 0; k < cols_a; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}

int main() {
    Matrix a = create_random_matrix(50, 60);
    Matrix b = create_random_matrix(60, 70);

    auto start = std::chrono::high_resolution_clock::now();
    Matrix result = matrix_mult(a, b);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time taken: " << elapsed.count() << " seconds\n";

    return 0;
}

