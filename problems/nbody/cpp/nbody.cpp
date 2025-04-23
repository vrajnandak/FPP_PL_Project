#include <iostream>
#include <vector>
#include <cmath>

const double G = 6.67430e-11;
const double dt = 0.01;

struct Body {
    double x, y;
    double vx, vy;
    double mass;
};

void compute_force(std::vector<Body>& bodies, int n) {
    std::vector<std::pair<double, double>> forces(n, {0, 0});

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;

            double dx = bodies[j].x - bodies[i].x;
            double dy = bodies[j].y - bodies[i].y;
            double dist = std::sqrt(dx * dx + dy * dy + 1e-10);
            double F = G * bodies[i].mass * bodies[j].mass / (dist * dist);
            double fx = F * dx / dist;
            double fy = F * dy / dist;

            forces[i].first += fx;
            forces[i].second += fy;
        }
    }

    for (int i = 0; i < n; ++i) {
        double ax = forces[i].first / bodies[i].mass;
        double ay = forces[i].second / bodies[i].mass;

        bodies[i].vx += ax * dt;
        bodies[i].vy += ay * dt;
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
    }
}

int main() {
    int n = 100, steps = 1000;
    std::vector<Body> bodies(n, {0, 0, 0, 0, 1});

    // initialize bodies with random positions
    for (int i = 0; i < n; ++i) {
        bodies[i].x = rand() % 1000;
        bodies[i].y = rand() % 1000;
        bodies[i].vx = 0;
        bodies[i].vy = 0;
        bodies[i].mass = 1e3;
    }

    for (int t = 0; t < steps; ++t)
        compute_force(bodies, n);

    std::cout << "Simulation complete.\n";
    return 0;
}
