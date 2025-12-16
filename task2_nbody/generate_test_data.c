#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 3.14159265358979323846
#define G 6.67430e-11 // Гравитационная постоянная

// Генерация тестовых данных
void generate_solar_system(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Ошибка: невозможно создать файл %s\n", filename);
        return;
    }

    // Упрощённая солнечная система (Солнце, Земля, Марс)
    fprintf(fp, "3\n");

    // Солнце (масса в кг, позиция в м, скорость в м/с)
    fprintf(fp, "1.989e30 0.0 0.0 0.0 0.0 0.0 0.0\n");

    // Земля
    fprintf(fp, "5.972e24 1.496e11 0.0 0.0 0.0 2.978e4 0.0\n");

    // Марс
    fprintf(fp, "6.39e23 2.279e11 0.0 0.0 0.0 2.407e4 0.0\n");

    fclose(fp);
    printf("Сгенерирован файл: %s (солнечная система из 3 тел)\n", filename);
}

void generate_random_bodies(const char *filename, int n)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Ошибка: невозможно создать файл %s\n", filename);
        return;
    }

    fprintf(fp, "%d\n", n);

    srand(42);

    for (int i = 0; i < n; i++)
    {
        // Случайная масса от 1e20 до 1e30 кг
        double mass = 1e20 * pow(10, 10.0 * ((double)rand() / RAND_MAX));

        // Случайная позиция внутри сферы радиуса 1e12 м
        double r = 1e12 * pow((double)rand() / RAND_MAX, 1.0 / 3.0);
        double theta = 2 * PI * ((double)rand() / RAND_MAX);
        double phi = acos(2 * ((double)rand() / RAND_MAX) - 1);

        double x = r * sin(phi) * cos(theta);
        double y = r * sin(phi) * sin(theta);
        double z = r * cos(phi);

        // Случайная скорость (приближение круговой орбиты)
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
    printf("Сгенерирован файл: %s (%d случайных тел)\n", filename, n);
}

void generate_binary_star(const char *filename)
{
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Ошибка: невозможно создать файл %s\n", filename);
        return;
    }

    fprintf(fp, "2\n");

    double mass = 1.989e30;     // Масса Солнца
    double distance = 1.496e11; // 1 астрономическая единица
    double velocity = sqrt(G * mass / distance);

    // Звезда 1
    fprintf(fp, "%.6e %.6e 0.0 0.0 0.0 %.6e 0.0\n",
            mass, -distance / 2, velocity / 2);

    // Звезда 2
    fprintf(fp, "%.6e %.6e 0.0 0.0 0.0 %.6e 0.0\n",
            mass, distance / 2, -velocity / 2);

    fclose(fp);
    printf("Сгенерирован файл: %s (двойная звёздная система)\n", filename);
}

int main()
{
    generate_solar_system("test_solar.txt");
    generate_binary_star("test_binary.txt");
    generate_random_bodies("test_10bodies.txt", 10);
    generate_random_bodies("test_50bodies.txt", 50);
    generate_random_bodies("test_100bodies.txt", 100);
    generate_random_bodies("test_500bodies.txt", 500);

    printf("\nГенерация тестовых данных завершена!\n");
    return 0;
}
