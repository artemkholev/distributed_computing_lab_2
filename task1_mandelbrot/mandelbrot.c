#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <complex.h>

#define MAX_ITER 1000
#define THRESHOLD 2.0

// Function to check if a point belongs to Mandelbrot set
int is_in_mandelbrot(double complex c) {
    double complex z = 0.0 + 0.0*I;
    int iter = 0;

    while (iter < MAX_ITER && cabs(z) < THRESHOLD) {
        z = z * z + c;
        iter++;
    }

    return (iter == MAX_ITER);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s nthreads npoints\n", argv[0]);
        return 1;
    }

    int nthreads = atoi(argv[1]);
    int npoints = atoi(argv[2]);

    if (nthreads <= 0 || npoints <= 0) {
        fprintf(stderr, "Error: nthreads and npoints must be positive integers\n");
        return 1;
    }

    omp_set_num_threads(nthreads);

    // Define the region to explore
    double real_min = -2.5;
    double real_max = 1.0;
    double imag_min = -1.0;
    double imag_max = 1.0;

    // Calculate grid size
    int points_per_dim = (int)sqrt(npoints);
    double real_step = (real_max - real_min) / points_per_dim;
    double imag_step = (imag_max - imag_min) / points_per_dim;

    // Open output file
    FILE *fp = fopen("mandelbrot_set.csv", "w");
    if (fp == NULL) {
        fprintf(stderr, "Error: Cannot open output file\n");
        return 1;
    }

    fprintf(fp, "real,imaginary\n");

    double start_time = omp_get_wtime();

    // Count points in set
    int total_points = 0;

    #pragma omp parallel
    {
        int local_count = 0;

        #pragma omp for schedule(dynamic)
        for (int i = 0; i < points_per_dim; i++) {
            for (int j = 0; j < points_per_dim; j++) {
                double real = real_min + i * real_step;
                double imag = imag_min + j * imag_step;
                double complex c = real + imag * I;

                if (is_in_mandelbrot(c)) {
                    #pragma omp critical
                    {
                        fprintf(fp, "%.10f,%.10f\n", real, imag);
                        local_count++;
                    }
                }
            }
        }

        #pragma omp atomic
        total_points += local_count;
    }

    double end_time = omp_get_wtime();

    fclose(fp);

    printf("Computation completed successfully\n");
    printf("Number of threads: %d\n", nthreads);
    printf("Total points checked: %d\n", points_per_dim * points_per_dim);
    printf("Points in Mandelbrot set: %d\n", total_points);
    printf("Execution time: %.6f seconds\n", end_time - start_time);
    printf("Output written to: mandelbrot_set.csv\n");

    return 0;
}
