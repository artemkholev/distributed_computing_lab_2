#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define G 6.67430e-11  // Gravitational constant

typedef struct {
    double x, y;
    double vx, vy;
    double mass;
} Body;

// Calculate forces between bodies using Newton's 3rd law
void calculate_forces(Body *bodies, int n, double *fx, double *fy) {
    // Initialize forces to zero
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        fx[i] = 0.0;
        fy[i] = 0.0;
    }

    // Calculate pairwise forces (using Newton's 3rd law to avoid redundant calculations)
    for (int q = 0; q < n; q++) {
        for (int k = q + 1; k < n; k++) {
            double dx = bodies[k].x - bodies[q].x;
            double dy = bodies[k].y - bodies[q].y;
            double dist_sq = dx * dx + dy * dy;
            double dist = sqrt(dist_sq);

            if (dist < 1e-10) continue; // Avoid division by zero

            double dist_cubed = dist_sq * dist;
            double force_mag = G * bodies[q].mass * bodies[k].mass / dist_cubed;

            double force_x = force_mag * dx;
            double force_y = force_mag * dy;

            // Newton's 3rd law: F_qk = -F_kq
            #pragma omp atomic
            fx[q] += force_x;
            #pragma omp atomic
            fy[q] += force_y;
            #pragma omp atomic
            fx[k] -= force_x;
            #pragma omp atomic
            fy[k] -= force_y;
        }
    }
}

// Update positions and velocities using Euler's method
void update_bodies(Body *bodies, int n, double *fx, double *fy, double dt) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        // Update velocities
        double ax = fx[i] / bodies[i].mass;
        double ay = fy[i] / bodies[i].mass;

        bodies[i].vx += ax * dt;
        bodies[i].vy += ay * dt;

        // Update positions
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s tend input_file nthreads\n", argv[0]);
        return 1;
    }

    double t_end = atof(argv[1]);
    char *input_file = argv[2];
    int nthreads = atoi(argv[3]);

    if (t_end <= 0 || nthreads <= 0) {
        fprintf(stderr, "Error: tend and nthreads must be positive\n");
        return 1;
    }

    omp_set_num_threads(nthreads);

    // Read input file
    FILE *fin = fopen(input_file, "r");
    if (fin == NULL) {
        fprintf(stderr, "Error: Cannot open input file %s\n", input_file);
        return 1;
    }

    int n;
    if (fscanf(fin, "%d", &n) != 1) {
        fprintf(stderr, "Error: Cannot read number of bodies\n");
        fclose(fin);
        return 1;
    }

    Body *bodies = (Body *)malloc(n * sizeof(Body));
    if (bodies == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(fin);
        return 1;
    }

    // Read initial conditions
    for (int i = 0; i < n; i++) {
        if (fscanf(fin, "%lf %lf %lf %lf %lf %lf",
                   &bodies[i].mass,
                   &bodies[i].x, &bodies[i].y,
                   &bodies[i].vx, &bodies[i].vy,
                   &bodies[i].vy) != 6) {
            // Actually read z coordinate but ignore it (2D simulation)
            double z, vz;
            fseek(fin, 0, SEEK_SET);
            fscanf(fin, "%d", &n);
            for (int j = 0; j <= i; j++) {
                if (fscanf(fin, "%lf %lf %lf %lf %lf %lf %lf",
                           &bodies[j].mass,
                           &bodies[j].x, &bodies[j].y, &z,
                           &bodies[j].vx, &bodies[j].vy, &vz) != 7) {
                    fprintf(stderr, "Error: Cannot read body %d\n", j);
                    free(bodies);
                    fclose(fin);
                    return 1;
                }
            }
        }
    }
    fclose(fin);

    // Open output file
    FILE *fout = fopen("trajectory_omp.csv", "w");
    if (fout == NULL) {
        fprintf(stderr, "Error: Cannot open output file\n");
        free(bodies);
        return 1;
    }

    // Write header
    fprintf(fout, "t");
    for (int i = 0; i < n; i++) {
        fprintf(fout, ",x%d,y%d", i + 1, i + 1);
    }
    fprintf(fout, "\n");

    // Time step (adaptive based on number of bodies)
    double dt = t_end / 1000.0;
    if (n > 100) dt = t_end / 500.0;

    double *fx = (double *)malloc(n * sizeof(double));
    double *fy = (double *)malloc(n * sizeof(double));

    if (fx == NULL || fy == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(bodies);
        fclose(fout);
        return 1;
    }

    double start_time = omp_get_wtime();

    // Simulation loop
    double t = 0.0;
    int step = 0;
    int output_interval = (int)(1000.0 / 100.0); // Output ~100 time points
    if (output_interval < 1) output_interval = 1;

    while (t <= t_end) {
        // Write current state
        if (step % output_interval == 0) {
            fprintf(fout, "%.6f", t);
            for (int i = 0; i < n; i++) {
                fprintf(fout, ",%.10f,%.10f", bodies[i].x, bodies[i].y);
            }
            fprintf(fout, "\n");
        }

        // Calculate forces
        calculate_forces(bodies, n, fx, fy);

        // Update bodies
        update_bodies(bodies, n, fx, fy, dt);

        t += dt;
        step++;
    }

    double end_time = omp_get_wtime();

    fclose(fout);
    free(fx);
    free(fy);
    free(bodies);

    printf("N-body simulation (OpenMP) completed successfully\n");
    printf("Number of bodies: %d\n", n);
    printf("Number of threads: %d\n", nthreads);
    printf("Simulation time: 0 to %.2f\n", t_end);
    printf("Time step: %.6f\n", dt);
    printf("Execution time: %.6f seconds\n", end_time - start_time);
    printf("Output written to: trajectory_omp.csv\n");

    return 0;
}
