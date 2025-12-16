# Лабораторная работа №2

### Компиляция всех заданий

```bash
# Задание 1
cd task1_mandelbrot && make && cd ..

# Задание 2
cd task2_nbody && make test_data && make nbody_omp && cd ..
# Для CUDA (если доступна): make nbody_cuda

# Задание 3
cd task3_rwlock && make && cd ..
```

## Задание 1: Множество Мандельброта (OpenMP)

Параллельный поиск точек множества Мандельброта с использованием OpenMP.

### Запуск

```bash
cd task1_mandelbrot
./mandelbrot <число_потоков> <число_точек>
```

**Примеры:**
```bash
./mandelbrot 4 100000     # 4 потока, 100,000 точек
./mandelbrot 8 1000000    # 8 потоков, 1,000,000 точек
```

**Выход:** Файл `mandelbrot_set.csv` с координатами точек множества.

## Задание 2: N-body симуляция (OpenMP и CUDA)

Моделирование движения N материальных точек под действием гравитации.

### Генерация тестовых данных

```bash
cd task2_nbody
make test_data
```

Создаются файлы:
- `test_solar.txt` - солнечная система (3 тела)
- `test_binary.txt` - двойная звезда (2 тела)
- `test_10bodies.txt`, `test_50bodies.txt`, `test_100bodies.txt`, `test_500bodies.txt` - случайные системы

### Запуск OpenMP версии

```bash
./nbody_omp <t_end> <файл_входных_данных> <число_потоков>
```

**Примеры:**
```bash
./nbody_omp 1.0 test_solar.txt 4       # Симуляция до t=1.0, 4 потока
./nbody_omp 10.0 test_100bodies.txt 8  # Симуляция до t=10.0, 8 потоков
```

**Выход:** Файл `trajectory_omp.csv` с траекториями всех тел.

### Запуск CUDA версии

```bash
make nbody_cuda  # Требуется CUDA Toolkit
./nbody_cuda <t_end> <файл_входных_данных>
```

**Пример:**
```bash
./nbody_cuda 1.0 test_100bodies.txt
```

**Выход:** Файл `trajectory_cuda.csv` с траекториями всех тел.

## Задание 3: Блокировки чтения-записи (pthreads)

Собственная реализация rwlock и сравнение со стандартной pthread_rwlock_t на примере потоко-безопасного односвязного списка.

### Ручной запуск

```bash
cd task3_rwlock

# Тест с собственной реализацией
./test_my_rwlock <число_потоков>

# Тест со стандартной реализацией
./test_pthread_rwlock <число_потоков>
```

При запуске программа запросит параметры:
1. Число ключей для инициализации списка
2. Общее число операций
3. Процент операций поиска (0.0 - 1.0)
4. Процент операций вставки (0.0 - 1.0)

**Пример:**
```bash
./test_my_rwlock 4
# Введите: 1000, 100000, 0.8, 0.1
```

### Автоматическое тестирование

```bash
chmod +x benchmark.sh
./benchmark.sh
```

Скрипт автоматически протестирует обе реализации с различным числом потоков (1, 2, 4, 8).

## Измерение производительности

### Задание 1: Mandelbrot

```bash
cd task1_mandelbrot

echo "=== Тест с разным числом потоков ==="
for threads in 1 2 4 8; do
    echo "Threads: $threads"
    ./mandelbrot $threads 1000000
    echo
done
```

### Задание 2: N-body

```bash
cd task2_nbody

echo "=== OpenMP: разное число потоков (100 тел) ==="
for threads in 1 2 4 8; do
    echo "Threads: $threads"
    ./nbody_omp 1.0 test_100bodies.txt $threads
    echo
done

echo "=== OpenMP vs CUDA: разное число тел ==="
for file in test_10bodies.txt test_50bodies.txt test_100bodies.txt test_500bodies.txt; do
    echo "File: $file"
    echo "OpenMP (4 threads):"
    ./nbody_omp 1.0 $file 4
    echo "CUDA:"
    ./nbody_cuda 1.0 $file
    echo
done
```

### Задание 3: rwlock

```bash
cd task3_rwlock
./benchmark.sh > results.txt
cat results.txt
```

## Очистка

```bash
# Очистка всех заданий
cd task1_mandelbrot && make clean && cd ..
cd task2_nbody && make clean && cd ..
cd task3_rwlock && make clean && cd ..
```

## Полный цикл тестирования

Скрипт для запуска всех тестов и сохранения результатов:

```bash
#!/bin/bash

echo "========================================="
echo "Лабораторная работа №2: Полное тестирование"
echo "========================================="
echo

# Задание 1
echo "=== Задание 1: Множество Мандельброта ==="
cd task1_mandelbrot
for threads in 1 2 4 8; do
    echo "Threads: $threads"
    ./mandelbrot $threads 100000
    echo
done
cd ..

# Задание 2
echo "=== Задание 2: N-body симуляция ==="
cd task2_nbody
echo "OpenMP версия:"
for threads in 1 2 4 8; do
    echo "Threads: $threads"
    ./nbody_omp 1.0 test_50bodies.txt $threads
    echo
done
cd ..

# Задание 3
echo "=== Задание 3: rwlock ==="
cd task3_rwlock
./benchmark.sh
cd ..

echo
echo "Тестирование завершено!"
```