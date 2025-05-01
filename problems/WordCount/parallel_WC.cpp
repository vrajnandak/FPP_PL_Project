// #include <iostream>
// #include <fstream>
// #include <sstream>
// #include <unordered_map>
// #include <vector>
// #include <omp.h>
// #include <cctype>

// vector<string> split_to_words(const string& text) {
//     istringstream iss(text);
//     string word;
//     vector<string> words;
//     while (iss >> word) {
//         string cleaned;
//         for (char c : word) {
//             if (isalpha(c)) cleaned += tolower(c);
//         }
//         if (!cleaned.empty()) words.push_back(cleaned);
//     }
//     return words;
// }

// int main() {
//     ifstream infile("input.txt");
//     stringstream buffer;
//     buffer << infile.rdbuf();
//     string text = buffer.str();

//     vector<string> words = split_to_words(text);

//     double start_time = omp_get_wtime();

//     int n = words.size();
//     int nthreads = omp_get_max_threads();
//     vector<unordered_map<string, int>> local_counts(nthreads);

//     #pragma omp parallel
//     {
//         int tid = omp_get_thread_num();
//         int chunk_size = (n + nthreads - 1) / nthreads;
//         int start = tid * chunk_size;
//         int end = min(n, (tid + 1) * chunk_size);

//         for (int i = start; i < end; ++i) {
//             local_counts[tid][words[i]]++;
//         }
//     }

//     unordered_map<string, int> global_counts;
//     for (const auto& lc : local_counts) {
//         for (const auto& [word, count] : lc) {
//             global_counts[word] += count;
//         }
//     }

//     double end_time = omp_get_wtime();
//     cout << "Parallel Time (OpenMP): " << (end_time - start_time) << " seconds\n";

//     return 0;
// }


#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <omp.h>
#include <cctype>
#include <chrono>

using namespace std;

vector<string> split_to_words(const string& text) {
    istringstream iss(text);
    string word;
    vector<string> words;
    while (iss >> word) {
        string cleaned;
        for (char c : word) {
            if (isalpha(c)) cleaned += tolower(c);
        }
        if (!cleaned.empty()) words.push_back(cleaned);
    }
    return words;
}

int main() {
    auto total_start = chrono::high_resolution_clock::now();
    
    // File reading phase
    auto file_read_start = chrono::high_resolution_clock::now();
    ifstream infile("input.txt");
    stringstream buffer;
    buffer << infile.rdbuf();
    string text = buffer.str();
    auto file_read_end = chrono::high_resolution_clock::now();
    
    // Word splitting phase
    auto split_start = chrono::high_resolution_clock::now();
    vector<string> words = split_to_words(text);
    auto split_end = chrono::high_resolution_clock::now();
    
    // Parallel computation setup
    auto comp_start = chrono::high_resolution_clock::now();
    int n = words.size();
    int nthreads = omp_get_max_threads();
    vector<unordered_map<string, int>> local_counts(nthreads);
    
    // Parallel computation
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int chunk_size = (n + nthreads - 1) / nthreads;
        int start = tid * chunk_size;
        int end = min(n, (tid + 1) * chunk_size);

        for (int i = start; i < end; ++i) {
            local_counts[tid][words[i]]++;
        }
    }
    auto comp_end = chrono::high_resolution_clock::now();
    
    // Reduction phase
    auto reduce_start = chrono::high_resolution_clock::now();
    unordered_map<string, int> global_counts;
    for (const auto& lc : local_counts) {
        for (const auto& [word, count] : lc) {
            global_counts[word] += count;
        }
    }
    auto reduce_end = chrono::high_resolution_clock::now();
    auto total_end = chrono::high_resolution_clock::now();
    
    // Print timings
    chrono::duration<double> file_read_time = file_read_end - file_read_start;
    chrono::duration<double> split_time = split_end - split_start;
    chrono::duration<double> comp_time = comp_end - comp_start;
    chrono::duration<double> reduce_time = reduce_end - reduce_start;
    chrono::duration<double> total_time = total_end - total_start;
    
    cout << "File Read Time     : " << file_read_time.count() << " seconds\n";
    cout << "Word Split Time    : " << split_time.count() << " seconds\n";
    cout << "Computation Time   : " << comp_time.count() << " seconds\n";
    cout << "Reduction Time     : " << reduce_time.count() << " seconds\n";
    cout << "Total Execution Time: " << total_time.count() << " seconds\n";
    
    // Optional: Print thread-specific timings
    #ifdef DETAILED_THREAD_TIMINGS
    #pragma omp parallel
    {
        #pragma omp master
        {
            cout << "Using " << omp_get_num_threads() << " threads\n";
        }
    }
    #endif

    return 0;
}