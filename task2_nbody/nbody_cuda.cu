#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda_runtime.h>

#define G 6.67430e-11  // Gravitational constant
#define BLOCK_SIZE 256

typedef struct {
    double x, y;
    double vx, vy;
    double mass;
} Body;

// CUDA kernel to calculate forces
__global__ void calculate_forces_kernel(Body *bodies, double *fx, double *fy, int n) {
    int q = blockIdx.x * blockDim.x + threadIdx.x;

    if (q < n) {
        double force_x = 0.0;
        double force_y = 0.0;

        for (int k = 0; k < n; k++) {
            if (k != q) {
                double dx = bodies[k].x - bodies[q].x;
                double dy = bodies[k].y - bodies[q].y;
                double dist_sq = dx * dx + dy * dy;

                if (dist_sq > 1e-20) {  // Avoid division by zero
                    double dist = sqrt(dist_sq);
                    double dist_cubed = dist_sq * dist;
                    double force_mag = G * bodies[q].mass * bodies[k].mass / dist_cubed;

                    force_x += force_mag * dx;
                    force_y += force_mag * dy;
                }
            }
        }

        fx[q] = force_x;
        fy[q] = force_y;
    }
}

// CUDA kernel to update positions and velocities
__global__ void update_bodies_kernel(Body *bodies, double *fx, double *fy, int n, double dt) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;

    if (i < n) {
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
    if (argc != 3) {
        fprintf(stderr, "Usage: %s tend input_file\n", argv[0]);
        return 1;
    }

    double t_end = atof(argv[1]);
    char *input_file = argv[2];

    if (t_end <= 0) {
        fprintf(stderr, "Error: tend must be positive\n");
        return 1;
    }

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

    Body *bodies_host = (Body *)malloc(n * sizeof(Body));
    if (bodies_host == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(fin);
        return 1;
    }

    // Read initial conditions (assuming 3D input but using only 2D)
    for (int i = 0; i < n; i++) {
        double z, vz;
        if (fscanf(fin, "%lf %lf %lf %lf %lf %lf %lf",
                   &bodies_host[i].mass,
                   &bodies_host[i].x, &bodies_host[i].y, &z,
                   &bodies_host[i].vx, &bodies_host[i].vy, &vz) != 7) {
            fprintf(stderr, "Error: Cannot read body %d\n", i);
            free(bodies_host);
            fclose(fin);
            return 1;
        }
    }
    fclose(fin);

    // Allocate device memory
    Body *bodies_dev;
    double *fx_dev, *fy_dev;

    cudaMalloc(&bodies_dev, n * sizeof(Body));
    cudaMalloc(&fx_dev, n * sizeof(double));
    cudaMalloc(&fy_dev, n * sizeof(double));

    // Copy data to device
    cudaMemcpy(bodies_dev, bodies_host, n * sizeof(Body), cudaMemcpyHostToDevice);

    // Open output file
    FILE *fout = fopen("trajectory_cuda.csv", "w");
    if (fout == NULL) {
        fprintf(stderr, "Error: Cannot open output file\n");
        free(bodies_host);
        cudaFree(bodies_dev);
        cudaFree(fx_dev);
        cudaFree(fy_dev);
        return 1;
    }

    // Write header
    fprintf(fout, "t");
    for (int i = 0; i < n; i++) {
        fprintf(fout, ",x%d,y%d", i + 1, i + 1);
    }
    fprintf(fout, "\n");

    // Time step
    double dt = t_end / 1000.0;
    if (n > 100) dt = t_end / 500.0;

    // CUDA grid configuration
    int num_blocks = (n + BLOCK_SIZE - 1) / BLOCK_SIZE;

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    // Simulation loop
    double t = 0.0;
    int step = 0;
    int output_interval = (int)(1000.0 / 100.0);
    if (output_interval < 1) output_interval = 1;

    while (t <= t_end) {
        // Copy data back to host for output
        if (step % output_interval == 0) {
            cudaMemcpy(bodies_host, bodies_dev, n * sizeof(Body), cudaMemcpyDeviceToHost);

            fprintf(fout, "%.6f", t);
            for (int i = 0; i < n; i++) {
                fprintf(fout, ",%.10f,%.10f", bodies_host[i].x, bodies_host[i].y);
            }
            fprintf(fout, "\n");
        }

        // Calculate forces
        calculate_forces_kernel<<<num_blocks, BLOCK_SIZE>>>(bodies_dev, fx_dev, fy_dev, n);

        // Update bodies
        update_bodies_kernel<<<num_blocks, BLOCK_SIZE>>>(bodies_dev, fx_dev, fy_dev, n, dt);

        cudaDeviceSynchronize();

        t += dt;
        step++;
    }

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    fclose(fout);

    // Cleanup
    cudaFree(bodies_dev);
    cudaFree(fx_dev);
    cudaFree(fy_dev);
    free(bodies_host);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    printf("N-body simulation (CUDA) completed successfully\n");
    printf("Number of bodies: %d\n", n);
    printf("Simulation time: 0 to %.2f\n", t_end);
    printf("Time step: %.6f\n", dt);
    printf("Execution time: %.6f seconds\n", milliseconds / 1000.0);
    printf("Output written to: trajectory_cuda.csv\n");

    return 0;
}
