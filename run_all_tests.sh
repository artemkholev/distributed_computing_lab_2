#!/bin/bash

echo "========================================="
echo "Лабораторная работа №2: Полное тестирование"
echo "========================================="
echo
echo "Дата: $(date)"
echo "Система: $(uname -a)"
echo "========================================="
echo

# Задание 1
echo "=== ЗАДАНИЕ 1: Множество Мандельброта (OpenMP) ==="
echo
cd task1_mandelbrot

if [ ! -f mandelbrot ]; then
    echo "Компиляция mandelbrot..."
    make
    echo
fi

echo "Тест 1: 100,000 точек, разное число потоков"
echo "-------------------------------------------"
for threads in 1 2 4 8; do
    echo -n "Threads=$threads: "
    ./mandelbrot $threads 100000 | grep "Execution time"
done
echo

echo "Тест 2: 1,000,000 точек, разное число потоков"
echo "---------------------------------------------"
for threads in 1 2 4 8; do
    echo -n "Threads=$threads: "
    ./mandelbrot $threads 1000000 | grep "Execution time"
done
echo
cd ..

# Задание 2
echo "=== ЗАДАНИЕ 2: N-body симуляция ==="
echo
cd task2_nbody

if [ ! -f nbody_omp ]; then
    echo "Компиляция nbody_omp..."
    make nbody_omp
    echo
fi

if [ ! -f test_10bodies.txt ]; then
    echo "Генерация тестовых данных..."
    make test_data
    echo
fi

echo "Тест 1: OpenMP, 50 тел, разное число потоков"
echo "--------------------------------------------"
for threads in 1 2 4 8; do
    echo -n "Threads=$threads: "
    ./nbody_omp 1.0 test_50bodies.txt $threads 2>/dev/null | grep "Execution time"
done
echo

echo "Тест 2: OpenMP, разное число тел (4 потока)"
echo "--------------------------------------------"
for file in test_10bodies.txt test_50bodies.txt test_100bodies.txt; do
    bodies=$(echo $file | grep -o '[0-9]*')
    echo -n "Bodies=$bodies: "
    ./nbody_omp 1.0 $file 4 2>/dev/null | grep "Execution time"
done
echo

if [ -f nbody_cuda ]; then
    echo "Тест 3: CUDA версия"
    echo "-------------------"
    for file in test_10bodies.txt test_50bodies.txt test_100bodies.txt; do
        bodies=$(echo $file | grep -o '[0-9]*')
        echo -n "Bodies=$bodies: "
        ./nbody_cuda 1.0 $file 2>/dev/null | grep "Execution time"
    done
    echo
else
    echo "CUDA версия не скомпилирована (пропускаем)"
    echo
fi

cd ..

# Задание 3
echo "=== ЗАДАНИЕ 3: Блокировки чтения-записи ==="
echo
cd task3_rwlock

if [ ! -f test_my_rwlock ] || [ ! -f test_pthread_rwlock ]; then
    echo "Компиляция rwlock тестов..."
    make
    echo
fi

echo "Параметры теста:"
echo "  Начальные ключи: 1000"
echo "  Общее число операций: 100,000"
echo "  Поиск: 80%, Вставка: 10%, Удаление: 10%"
echo

echo "Тест: Сравнение my_rwlock vs pthread_rwlock"
echo "-------------------------------------------"

for threads in 1 2 4 8; do
    echo "Threads=$threads:"

    echo -n "  my_rwlock:      "
    echo -e "1000\n100000\n0.8\n0.1" | ./test_my_rwlock $threads 2>/dev/null | grep "Elapsed time"

    echo -n "  pthread_rwlock: "
    echo -e "1000\n100000\n0.8\n0.1" | ./test_pthread_rwlock $threads 2>/dev/null | grep "Elapsed time"

    echo
done

cd ..

echo "========================================="
echo "Тестирование завершено!"
echo "========================================="
echo
echo "Для детального анализа результатов см. ОТЧЕТ.md"
