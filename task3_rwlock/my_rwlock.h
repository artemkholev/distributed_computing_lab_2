#ifndef MY_RWLOCK_H
#define MY_RWLOCK_H

#include <pthread.h>

typedef struct
{
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int active_readers;  // Количество читающих потоков
    int active_writer;   // Флаг: 1 если писатель активен, 0 иначе
    int waiting_readers; // Количество потоков, ожидающих чтения
    int waiting_writers; // Количество потоков, ожидающих записи
} my_rwlock_t;

// Инициализация rwlock
int my_rwlock_init(my_rwlock_t *rwlock);

// Уничтожение rwlock
int my_rwlock_destroy(my_rwlock_t *rwlock);

// Получение блокировки на чтение
int my_rwlock_rdlock(my_rwlock_t *rwlock);

// Получение блокировки на запись
int my_rwlock_wrlock(my_rwlock_t *rwlock);

// Освобождение блокировки (чтение и запись)
int my_rwlock_unlock(my_rwlock_t *rwlock);

#endif // MY_RWLOCK_H
