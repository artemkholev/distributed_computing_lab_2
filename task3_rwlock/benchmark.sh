#!/bin/bash

# Скрипт бенчмарка для сравнения собственного rwlock и pthread_rwlock
THREADS=(1 2 4 8)
INITIAL_KEYS=1000
TOTAL_OPS=100000
SEARCH_PERCENT=0.8
INSERT_PERCENT=0.1

echo "Конфигурация теста:"
echo "  Начальное количество ключей: $INITIAL_KEYS"
echo "  Общее количество операций: $TOTAL_OPS"
echo "  Доля операций поиска: $SEARCH_PERCENT"
echo "  Доля операций вставки: $INSERT_PERCENT"
echo "  Доля операций удаления: $(echo "1.0 - $SEARCH_PERCENT - $INSERT_PERCENT" | bc)"
echo

for threads in "${THREADS[@]}"; do
    echo "Тестирование с $threads потоком(ами)"

    echo
    echo "--- Пользовательская реализация my_rwlock ---"
    echo -e "$INITIAL_KEYS\n$TOTAL_OPS\n$SEARCH_PERCENT\n$INSERT_PERCENT" | ./test_my_rwlock $threads

    echo
    echo "--- Стандартный pthread_rwlock ---"
    echo -e "$INITIAL_KEYS\n$TOTAL_OPS\n$SEARCH_PERCENT\n$INSERT_PERCENT" | ./test_pthread_rwlock $threads

    echo
done
