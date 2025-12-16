#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <string.h>

#define G 6.67430e-11 // Гравитационная постоянная

typedef struct
{
    double x, y;
    double vx, vy;
    double mass;
} Body;

// Вычисление сил между телами используя 3-й закон Ньютона
void calculate_forces(Body *bodies, int n, double *fx, double *fy)
{
// Инициализация сил нулями
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        fx[i] = 0.0;
        fy[i] = 0.0;
    }

    // Расчёт попарных сил (используя 3-й закон Ньютона для избежания лишних вычислений)
    for (int q = 0; q < n; q++)
    {
        for (int k = q + 1; k < n; k++)
        {
            double dx = bodies[k].x - bodies[q].x;
            double dy = bodies[k].y - bodies[q].y;
            double dist_sq = dx * dx + dy * dy;
            double dist = sqrt(dist_sq);

            if (dist < 1e-10)
                continue; // Избегаем деления на ноль

            double dist_cubed = dist_sq * dist;
            double force_mag = G * bodies[q].mass * bodies[k].mass / dist_cubed;

            double force_x = force_mag * dx;
            double force_y = force_mag * dy;

// 3-й закон Ньютона: F_qk = -F_kq
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

// Обновление координат и скоростей методом Эйлера
void update_bodies(Body *bodies, int n, double *fx, double *fy, double dt)
{
#pragma omp parallel for
    for (int i = 0; i < n; i++)
    {
        // Обновление скоростей
        double ax = fx[i] / bodies[i].mass;
        double ay = fy[i] / bodies[i].mass;

        bodies[i].vx += ax * dt;
        bodies[i].vy += ay * dt;

        // Обновление координат
        bodies[i].x += bodies[i].vx * dt;
        bodies[i].y += bodies[i].vy * dt;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Использование: %s t_end входной_файл количество_потоков\n", argv[0]);
        return 1;
    }

    double t_end = atof(argv[1]);
    char *input_file = argv[2];
    int nthreads = atoi(argv[3]);

    if (t_end <= 0 || nthreads <= 0)
    {
        fprintf(stderr, "Ошибка: t_end и количество_потоков должны быть положительными\n");
        return 1;
    }

    omp_set_num_threads(nthreads);

    // Чтение входного файла
    FILE *fin = fopen(input_file, "r");
    if (fin == NULL)
    {
        fprintf(stderr, "Ошибка: не удалось открыть входной файл %s\n", input_file);
        return 1;
    }

    int n;
    if (fscanf(fin, "%d", &n) != 1)
    {
        fprintf(stderr, "Ошибка: не удалось прочитать количество тел\n");
        fclose(fin);
        return 1;
    }

    Body *bodies = (Body *)malloc(n * sizeof(Body));
    if (bodies == NULL)
    {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        fclose(fin);
        return 1;
    }

    // Чтение начальных условий
    for (int i = 0; i < n; i++)
    {
        if (fscanf(fin, "%lf %lf %lf %lf %lf %lf",
                   &bodies[i].mass,
                   &bodies[i].x, &bodies[i].y,
                   &bodies[i].vx, &bodies[i].vy,
                   &bodies[i].vy) != 6)
        {
            // Читаем z координату, но игнорируем её (2D симуляция)
            double z, vz;
            fseek(fin, 0, SEEK_SET);
            fscanf(fin, "%d", &n);
            for (int j = 0; j <= i; j++)
            {
                if (fscanf(fin, "%lf %lf %lf %lf %lf %lf %lf",
                           &bodies[j].mass,
                           &bodies[j].x, &bodies[j].y, &z,
                           &bodies[j].vx, &bodies[j].vy, &vz) != 7)
                {
                    fprintf(stderr, "Ошибка: не удалось прочитать тело %d\n", j);
                    free(bodies);
                    fclose(fin);
                    return 1;
                }
            }
        }
    }
    fclose(fin);

    // Открытие выходного файла
    FILE *fout = fopen("trajectory_omp.csv", "w");
    if (fout == NULL)
    {
        fprintf(stderr, "Ошибка: не удалось открыть выходной файл\n");
        free(bodies);
        return 1;
    }

    // Запись заголовка
    fprintf(fout, "t");
    for (int i = 0; i < n; i++)
    {
        fprintf(fout, ",x%d,y%d", i + 1, i + 1);
    }
    fprintf(fout, "\n");

    // Шаг по времени (адаптивный в зависимости от количества тел)
    double dt = t_end / 1000.0;
    if (n > 100)
        dt = t_end / 500.0;

    double *fx = (double *)malloc(n * sizeof(double));
    double *fy = (double *)malloc(n * sizeof(double));

    if (fx == NULL || fy == NULL)
    {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        free(bodies);
        fclose(fout);
        return 1;
    }

    double start_time = omp_get_wtime();

    // Цикл симуляции
    double t = 0.0;
    int step = 0;
    int output_interval = (int)(1000.0 / 100.0); // Вывод ~100 временных точек
    if (output_interval < 1)
        output_interval = 1;

    while (t <= t_end)
    {
        // Запись текущего состояния
        if (step % output_interval == 0)
        {
            fprintf(fout, "%.6f", t);
            for (int i = 0; i < n; i++)
            {
                fprintf(fout, ",%.10f,%.10f", bodies[i].x, bodies[i].y);
            }
            fprintf(fout, "\n");
        }

        // Вычисление сил
        calculate_forces(bodies, n, fx, fy);

        // Обновление тел
        update_bodies(bodies, n, fx, fy, dt);

        t += dt;
        step++;
    }

    double end_time = omp_get_wtime();

    fclose(fout);
    free(fx);
    free(fy);
    free(bodies);

    printf("N-body симуляция (OpenMP) успешно завершена\n");
    printf("Количество тел: %d\n", n);
    printf("Количество потоков: %d\n", nthreads);
    printf("Время симуляции: от 0 до %.2f\n", t_end);
    printf("Шаг по времени: %.6f\n", dt);
    printf("Время выполнения: %.6f секунд\n", end_time - start_time);
    printf("Результат записан в: trajectory_omp.csv\n");

    return 0;
}
