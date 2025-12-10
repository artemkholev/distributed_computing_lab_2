#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define G 6.67430e-11  // Gravitational constant

// Generate test data for N-body simulation
void generate_solar_system(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot create file %s\n", filename);
        return;
    }

    // Simplified solar system (Sun, Earth, Mars)
    fprintf(fp, "3\n");

    // Sun (mass in kg, position in m, velocity in m/s)
    fprintf(fp, "1.989e30 0.0 0.0 0.0 0.0 0.0 0.0\n");

    // Earth
    fprintf(fp, "5.972e24 1.496e11 0.0 0.0 0.0 2.978e4 0.0\n");

    // Mars
    fprintf(fp, "6.39e23 2.279e11 0.0 0.0 0.0 2.407e4 0.0\n");

    fclose(fp);
    printf("Generated: %s (3-body solar system)\n", filename);
}

void generate_random_bodies(const char *filename, int n) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot create file %s\n", filename);
        return;
    }

    fprintf(fp, "%d\n", n);

    srand(42); // Fixed seed for reproducibility

    for (int i = 0; i < n; i++) {
        // Random mass between 1e20 and 1e30 kg
        double mass = 1e20 * pow(10, 10.0 * ((double)rand() / RAND_MAX));

        // Random position in a sphere of radius 1e12 m
        double r = 1e12 * pow((double)rand() / RAND_MAX, 1.0/3.0);
        double theta = 2 * PI * ((double)rand() / RAND_MAX);
        double phi = acos(2 * ((double)rand() / RAND_MAX) - 1);

        double x = r * sin(phi) * cos(theta);
        double y = r * sin(phi) * sin(theta);
        double z = r * cos(phi);

        // Random velocity (circular orbit approximation)
        double v = 1e4 * ((double)rand() / RAND_MAX);
        double vtheta = 2 * PI * ((double)rand() / RAND_MAX);
        double vphi = acos(2 * ((double)rand() / RAND_MAX) - 1);

        double vx = v * sin(vphi) * cos(vtheta);
        double vy = v * sin(vphi) * sin(vtheta);
        double vz = v * cos(vphi);

        fprintf(fp, "%.6e %.6e %.6e %.6e %.6e %.6e %.6e\n",
                mass, x, y, z, vx, vy, vz);
    }

    fclose(fp);
    printf("Generated: %s (%d random bodies)\n", filename, n);
}

void generate_binary_star(const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot create file %s\n", filename);
        return;
    }

    fprintf(fp, "2\n");

    double mass = 1.989e30; // Solar mass
    double distance = 1.496e11; // 1 AU
    double velocity = sqrt(G * mass / distance);

    // Star 1
    fprintf(fp, "%.6e %.6e 0.0 0.0 0.0 %.6e 0.0\n", mass, -distance/2, velocity/2);

    // Star 2
    fprintf(fp, "%.6e %.6e 0.0 0.0 0.0 %.6e 0.0\n", mass, distance/2, -velocity/2);

    fclose(fp);
    printf("Generated: %s (binary star system)\n", filename);
}

int main() {
    generate_solar_system("test_solar.txt");
    generate_binary_star("test_binary.txt");
    generate_random_bodies("test_10bodies.txt", 10);
    generate_random_bodies("test_50bodies.txt", 50);
    generate_random_bodies("test_100bodies.txt", 100);
    generate_random_bodies("test_500bodies.txt", 500);

    printf("\nTest data generation complete!\n");
    return 0;
}
