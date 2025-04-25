// File: mcm_cpp.cpp
#include <iostream>
#include <vector>
#include <future>
#include <limits>
#include <numeric> // for std::iota (optional)
#include <algorithm> // for std::min
#include <stdexcept> // for std::runtime_error
#include <chrono>    // For timing (optional)

// Helper function to compute the cost for a single entry m[i][j]
// It reads previously computed values from the shared 'm' table.
int compute_entry(int i, int j, const std::vector<int>& p, const std::vector<std::vector<int>>& m) {
    long long min_cost_ll = std::numeric_limits<long long>::max();
    int n = p.size() - 1; // Needed for dimension access p[k]

    if (i < 1 || i > n || j < i || j > n) {
         throw std::runtime_error("Invalid indices in compute_entry");
    }

    for (int k = i; k < j; ++k) {
        // Check indices for 'm' access
        if (k < i || k+1 > j || k+1 < 1) {
             throw std::runtime_error("Invalid k index");
        }
        // Check indices for 'p' access
        if (i - 1 < 0 || i - 1 >= p.size() || k < 0 || k >= p.size() || j < 0 || j >= p.size()) {
             throw std::runtime_error("Invalid dimension index");
        }

        // Ensure m values are non-negative (or handle sentinel if used)
        if (m[i][k] < 0 || m[k + 1][j] < 0) {
            // This shouldn't happen if max_int was used correctly for uncomputed/infinite
             if (m[i][k] == std::numeric_limits<int>::max() || m[k + 1][j] == std::numeric_limits<int>::max()) {
                continue; // Skip if subproblem cost is infinite
             }
            throw std::runtime_error("Negative cost encountered in subproblem");
        }

        // Use long long for intermediate calculation to prevent overflow
        long long term1 = m[i][k];
        long long term2 = m[k + 1][j];
        long long term3 = static_cast<long long>(p[i - 1]) * p[k] * p[j];

        // Check for potential overflow before adding
        if (term1 > std::numeric_limits<long long>::max() - term2) {
             // Overflow would occur on term1 + term2
             continue; // Skip this k value
        }
        long long sum12 = term1 + term2;
         if (term3 > std::numeric_limits<long long>::max() - sum12) {
             // Overflow would occur on sum12 + term3
             continue; // Skip this k value
         }

        long long current_cost_ll = sum12 + term3;
        min_cost_ll = std::min(min_cost_ll, current_cost_ll);
    }

    // Check if the result fits in int
    if (min_cost_ll > std::numeric_limits<int>::max()) {
        return std::numeric_limits<int>::max();
    }
    // Handle case where no valid 'k' was found (should only happen if subproblems were max_int)
    if (min_cost_ll == std::numeric_limits<long long>::max()) {
         return std::numeric_limits<int>::max();
    }

    return static_cast<int>(min_cost_ll);
}

// Parallel MCM using std::async and std::future
int matrix_chain_order_async(const std::vector<int>& p) {
    int n = p.size() - 1;
    if (n <= 0) return 0;

    // DP table: m[i][j] stores the min cost for matrices Ai...Aj
    // Initialize with 0. m[i][i] = 0 is correct. Other values will be overwritten.
    std::vector<std::vector<int>> m(n + 1, std::vector<int>(n + 1, 0));

    // Iterate over chain lengths
    for (int len = 2; len <= n; ++len) {
        std::vector<std::future<int>> futures;
        // Store indices to update 'm' later, corresponding to futures
        std::vector<std::pair<int, int>> indices;

        // Launch tasks for all subproblems of the current length 'len'
        for (int i = 1; i <= n - len + 1; ++i) {
            int j = i + len - 1;
            indices.emplace_back(i, j); // Store the target indices

            // Launch asynchronously. Pass 'm' by const reference because
            // compute_entry only needs read access to *previously* computed values.
            // Use std::launch::async policy to suggest thread creation.
            futures.push_back(std::async(std::launch::async, compute_entry, i, j, std::cref(p), std::cref(m)));
        }

        // Wait for all tasks for the current length and collect results
        for (size_t k = 0; k < futures.size(); ++k) {
            try {
                int result = futures[k].get(); // Waits for the future, throws if task threw exception
                int i = indices[k].first;
                int j = indices[k].second;
                m[i][j] = result; // Update the DP table
            } catch (const std::exception& e) {
                std::cerr << "Error computing m[" << indices[k].first << "][" << indices[k].second << "]: " << e.what() << std::endl;
                // Decide how to handle errors, e.g., rethrow, return error code, etc.
                // Setting to max_int might be an option if partial results are acceptable.
                 m[indices[k].first][indices[k].second] = std::numeric_limits<int>::max();
            }
        }
         // At this point, all m[i][j] for the current 'len' are computed.
    }

    return m[1][n];
}


// Standard Sequential MCM (for comparison)
int matrix_chain_order_seq(const std::vector<int>& p) {
    int n = p.size() - 1;
    if (n <= 0) return 0;
    std::vector<std::vector<int>> m(n + 1, std::vector<int>(n + 1, 0));

    for (int len = 2; len <= n; ++len) {
        for (int i = 1; i <= n - len + 1; ++i) {
            int j = i + len - 1;
            m[i][j] = std::numeric_limits<int>::max(); // Initialize with infinity
            for (int k = i; k < j; ++k) {
                 // Use long long for intermediate calculation
                 long long cost_ll = static_cast<long long>(m[i][k])
                                    + m[k + 1][j]
                                    + static_cast<long long>(p[i - 1]) * p[k] * p[j];

                 // Clamp to int max if overflow occurs
                 int cost = (cost_ll > std::numeric_limits<int>::max())
                            ? std::numeric_limits<int>::max()
                            : static_cast<int>(cost_ll);

                 if (cost < m[i][j]) {
                    m[i][j] = cost;
                 }
            }
        }
    }
    return m[1][n];
}

// int main() {
//     std::vector<int> p = {30, 35, 15, 5, 10, 20, 25}; // Example dimensions

//     std::cout << "Dimensions: ";
//     for (size_t i = 0; i < p.size(); ++i) {
//         std::cout << p[i] << (i == p.size() - 1 ? "" : " x ");
//     }
//     std::cout << std::endl;

//     try {
//         // Time sequential version (optional)
//         auto start_seq = std::chrono::high_resolution_clock::now();
//         int cost_seq = matrix_chain_order_seq(p);
//         auto end_seq = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double, std::milli> duration_seq = end_seq - start_seq;
//         std::cout << "Sequential Min Cost: " << cost_seq << " (took " << duration_seq.count() << " ms)" << std::endl;


//         // Time parallel version
//         auto start_async = std::chrono::high_resolution_clock::now();
//         int cost_async = matrix_chain_order_async(p);
//         auto end_async = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double, std::milli> duration_async = end_async - start_async;
//         std::cout << "Async Min Cost:      " << cost_async << " (took " << duration_async.count() << " ms)" << std::endl;

//         if(cost_seq != cost_async) {
//             std::cerr << "Warning: Sequential and Async results differ!" << std::endl;
//         }

//     } catch (const std::exception& e) {
//         std::cerr << "An error occurred: " << e.what() << std::endl;
//         return 1;
//     }

//     return 0;
// }

#include <random> // Include this at the top with other headers!

int main() {
    const int num_matrices = 200; // You can increase this to 500, 1000, etc.
    const int min_dim = 5;         // Minimum dimension size
    const int max_dim = 100;       // Maximum dimension size

    std::vector<int> p(num_matrices + 1); // Need n+1 dimensions for n matrices

    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd()); // Mersenne Twister RNG
    std::uniform_int_distribution<> dis(min_dim, max_dim);

    for (int i = 0; i <= num_matrices; ++i) {
        p[i] = dis(gen);
    }

    std::cout << "Number of matrices: " << num_matrices << std::endl;
    std::cout << "Sample dimensions (first 10): ";
    for (int i = 0; i < std::min(10, (int)p.size()); ++i) {
        std::cout << p[i] << (i == std::min(10, (int)p.size()) - 1 ? "" : " x ");
    }
    if (p.size() > 10) std::cout << " ...";
    std::cout << std::endl;

    try {
        // Time sequential version
        auto start_seq = std::chrono::high_resolution_clock::now();
        int cost_seq = matrix_chain_order_seq(p);
        auto end_seq = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_seq = end_seq - start_seq;
        std::cout << "Sequential Min Cost: " << cost_seq << " (took " << duration_seq.count() << " ms)" << std::endl;

        // Time parallel version
        auto start_async = std::chrono::high_resolution_clock::now();
        int cost_async = matrix_chain_order_async(p);
        auto end_async = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration_async = end_async - start_async;
        std::cout << "Async Min Cost:      " << cost_async << " (took " << duration_async.count() << " ms)" << std::endl;

        if (cost_seq != cost_async) {
            std::cerr << "Warning: Sequential and Async results differ!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
