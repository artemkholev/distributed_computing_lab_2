CC = gcc
NVCC = nvcc
CFLAGS = -fopenmp -O3 -Wall
LDFLAGS = -lm
NVCCFLAGS = -O3

all: nbody_omp nbody_cuda

nbody_omp: nbody_omp.c
	$(CC) $(CFLAGS) -o nbody_omp nbody_omp.c $(LDFLAGS)

nbody_cuda: nbody_cuda.cu
	$(NVCC) $(NVCCFLAGS) -o nbody_cuda nbody_cuda.cu

test_data: generate_test_data.c
	$(CC) -o generate_test_data generate_test_data.c -lm
	./generate_test_data

clean:
	rm -f nbody_omp nbody_cuda generate_test_data
	rm -f trajectory_omp.csv trajectory_cuda.csv
	rm -f test_*.txt

.PHONY: all clean test_data
