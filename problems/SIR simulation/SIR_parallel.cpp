#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <omp.h>

enum class State { Susceptible, Infected, Recovered };

struct Person {
    State state;
};

using Population = std::vector<std::vector<Person*>>;

void printPopulation(const Population& pop) {
    for (const auto& row : pop) {
        for (const auto* person : row) {
            if (person) {
                switch (person->state) {
                    case State::Susceptible: std::cout << "S "; break;
                    case State::Infected: std::cout << "I "; break;
                    case State::Recovered: std::cout << "R "; break;
                }
            } else {
                std::cout << ". ";
            }
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

Population initializePopulation(int rows, int cols, float density) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    Population pop(rows, std::vector<Person*>(cols, nullptr));
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (dis(gen) < density) {
                pop[i][j] = new Person{State::Susceptible};
            }
        }
    }
    return pop;
}

void movePopulation(Population& pop, float moveProb = 0.5) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    std::uniform_int_distribution<> dir(0, 3);
    
    int rows = pop.size();
    if (rows == 0) return;
    int cols = pop[0].size();
    
    // Directions: right, down, left, up
    const int directions[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    
    // Create a copy for reading while we modify the original
    Population newPop = pop;
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (pop[i][j] && dis(gen) < moveProb) {
                int d = dir(gen);
                int ni = i + directions[d][0];
                int nj = j + directions[d][1];
                
                if (ni >= 0 && ni < rows && nj >= 0 && nj < cols && !newPop[ni][nj]) {
                    newPop[ni][nj] = pop[i][j];
                    newPop[i][j] = nullptr;
                }
            }
        }
    }
    
    pop = std::move(newPop);
}

Population step(float beta, float gamma, const Population& pop) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);
    
    int rows = pop.size();
    if (rows == 0) return {};
    int cols = pop[0].size();
    
    // Create new population with copied states
    Population newPop(rows, std::vector<Person*>(cols, nullptr));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (pop[i][j]) {
                newPop[i][j] = new Person{pop[i][j]->state};
            }
        }
    }
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!pop[i][j]) continue;
            
            switch (pop[i][j]->state) {
                case State::Susceptible: {
                    int infectedNeighbors = 0;
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            if (di == 0 && dj == 0) continue;
                            int ni = i + di;
                            int nj = j + dj;
                            if (ni >= 0 && ni < rows && nj >= 0 && nj < cols && 
                                pop[ni][nj] && pop[ni][nj]->state == State::Infected) {
                                infectedNeighbors++;
                            }
                        }
                    }
                    float prob = 1.0 - std::pow(1.0 - beta, infectedNeighbors);
                    if (dis(gen) < prob) {
                        newPop[i][j]->state = State::Infected;
                    }
                    break;
                }
                case State::Infected:
                    if (dis(gen) < gamma) {
                        newPop[i][j]->state = State::Recovered;
                    }
                    break;
                case State::Recovered:
                    // No change
                    break;
            }
        }
    }
    
    // printPopulation(newPop);
    return newPop;
}

int simulate(float beta, float gamma, int rows, int cols, int steps, float density) {
    auto pop = initializePopulation(rows, cols, density);
    
    // Patient zero
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> rowDist(0, rows-1);
    std::uniform_int_distribution<> colDist(0, cols-1);
    
    while (true) {
        int i = rowDist(gen);
        int j = colDist(gen);
        if (pop[i][j]) {
            pop[i][j]->state = State::Infected;
            break;
        }
    }
    
    for (int s = 0; s < steps; ++s) {
        auto updated = step(beta, gamma, pop);
        movePopulation(updated);
        pop = std::move(updated);
    }
    
    int infectedCount = 0;
    for (const auto& row : pop) {
        for (const auto* person : row) {
            if (person && person->state == State::Infected) {
                infectedCount++;
            }
        }
    }
    
    // Clean up memory
    for (auto& row : pop) {
        for (auto* person : row) {
            delete person;
        }
    }
    
    return infectedCount;
}

int main() {
    const int runs = 100;
    const float beta = 0.2f;
    const float gamma = 0.05f;
    const int rows = 50;
    const int cols = 50;
    const int steps = 50;
    const float density = 0.7f;
    
    double t1 = omp_get_wtime();
    
    std::vector<int> infections(runs);

    #pragma omp parallel for
    for (int i = 0; i < runs; ++i) {
        infections[i] = simulate(beta, gamma, rows, cols, steps, density);
    }
    
    double t2 = omp_get_wtime();
    
    for (int i = 0; i < runs; ++i) {
        std::cout << "Simulation " << (i+1) << ": " << infections[i] << " infections\n";
    }
    
    float avg = 0.0f;
    for (int count : infections) {
        avg += count;
    }
    avg /= runs;
    
    std::cout << "\nParallel: Time taken = " << (t2 - t1) << " seconds\n";
    std::cout << "Average infections at end of simulation: " << avg << "\n";
    
    return 0;
}