#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>  // OpenMP

enum class State { Susceptible, Infected, Recovered };

struct Person {
    State state = State::Susceptible;
};

using Population = std::vector<std::vector<Person>>;

void step(double beta, double gamma, Population& pop, std::mt19937& rng) {
    int rows = pop.size();
    int cols = pop[0].size();
    std::uniform_real_distribution<> dist(0.0, 1.0);

    Population next_pop = pop; // Copy for updating in parallel safely

#pragma omp parallel for collapse(2)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            auto& person = pop[i][j];
            if (person.state == State::Susceptible) {
                int infected_neighbors = 0;
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        if (di == 0 && dj == 0) continue;
                        int ni = i + di;
                        int nj = j + dj;
                        if (ni >= 0 && ni < rows && nj >= 0 && nj < cols) {
                            if (pop[ni][nj].state == State::Infected) {
                                infected_neighbors++;
                            }
                        }
                    }
                }
                double prob = 1.0 - pow(1.0 - beta, infected_neighbors);
                if (dist(rng) < prob) {
                    next_pop[i][j].state = State::Infected;
                }
            } else if (person.state == State::Infected) {
                if (dist(rng) < gamma) {
                    next_pop[i][j].state = State::Recovered;
                }
            }
        }
    }

    pop = next_pop; // Update
}

int simulate(double beta, double gamma, int rows, int cols, int steps, unsigned int seed) {
    Population pop(rows, std::vector<Person>(cols));
    pop[rows/2][cols/2].state = State::Infected;
    std::mt19937 rng(seed);

    for (int step_num = 0; step_num < steps; ++step_num) {
        step(beta, gamma, pop, rng);
    }

    int infected = 0;
    for (const auto& row : pop) {
        for (const auto& p : row) {
            if (p.state == State::Infected) infected++;
        }
    }
    return infected;
}

int main() {
    const int runs = 1000;
    const double beta = 0.2;
    const double gamma = 0.1;
    const int rows = 50;
    const int cols = 50;
    const int steps = 50;

    std::vector<int> infections(runs);

    auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for
    for (int i = 0; i < runs; ++i) {
        unsigned int seed = i + time(0);  // Different seed per thread
        infections[i] = simulate(beta, gamma, rows, cols, steps, seed);
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;

    double avg = 0.0;
    for (auto inf : infections) avg += inf;
    avg /= runs;

    // for (int i = 0; i < runs; ++i) {
    //     std::cout << "Simulation " << i+1 << ": " << infections[i] << " infections\n";
    // }

    std::cout << "Average infections at end of simulation: " << avg << "\n";
    std::cout << "Time taken: " << diff.count() << " seconds\n";

    return 0;
}