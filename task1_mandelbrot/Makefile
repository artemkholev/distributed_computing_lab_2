CC = gcc
CFLAGS = -fopenmp -O3 -Wall
LDFLAGS = -lm
TARGET = mandelbrot

all: $(TARGET)

$(TARGET): mandelbrot.c
	$(CC) $(CFLAGS) -o $(TARGET) mandelbrot.c $(LDFLAGS)

clean:
	rm -f $(TARGET) mandelbrot_set.csv

run: $(TARGET)
	./$(TARGET) 4 1000000

.PHONY: all clean run
