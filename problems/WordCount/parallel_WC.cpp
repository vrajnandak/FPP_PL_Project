#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <cctype>

std::vector<std::string> split_to_words(const std::string& text) {
    std::istringstream iss(text);
    std::string word;
    std::vector<std::string> words;
    while (iss >> word) {
        std::string cleaned;
        for (char c : word) {
            if (std::isalpha(c)) cleaned += std::tolower(c);
        }
        if (!cleaned.empty()) words.push_back(cleaned);
    }
    return words;
}

int main() {
    std::ifstream infile("input.txt");
    std::stringstream buffer;
    buffer << infile.rdbuf();
    std::string text = buffer.str();

    std::vector<std::string> words = split_to_words(text);

    double start_time = omp_get_wtime();

    int n = words.size();
    int nthreads = omp_get_max_threads();
    std::vector<std::unordered_map<std::string, int>> local_counts(nthreads);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int chunk_size = (n + nthreads - 1) / nthreads;
        int start = tid * chunk_size;
        int end = std::min(n, (tid + 1) * chunk_size);

        for (int i = start; i < end; ++i) {
            local_counts[tid][words[i]]++;
        }
    }

    std::unordered_map<std::string, int> global_counts;
    for (const auto& lc : local_counts) {
        for (const auto& [word, count] : lc) {
            global_counts[word] += count;
        }
    }

    double end_time = omp_get_wtime();
    std::cout << "Parallel Time (OpenMP): " << (end_time - start_time) << " seconds\n";

    return 0;
}
