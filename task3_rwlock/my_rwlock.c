#include "my_rwlock.h"
#include <stdio.h>

int my_rwlock_init(my_rwlock_t *rwlock)
{
    if (rwlock == NULL)
    {
        return -1;
    }

    // Инициализация мьютекса
    if (pthread_mutex_init(&rwlock->mutex, NULL) != 0)
    {
        return -1;
    }

    // Инициализация условных переменных
    if (pthread_cond_init(&rwlock->readers_cond, NULL) != 0)
    {
        pthread_mutex_destroy(&rwlock->mutex);
        return -1;
    }

    if (pthread_cond_init(&rwlock->writers_cond, NULL) != 0)
    {
        pthread_mutex_destroy(&rwlock->mutex);
        pthread_cond_destroy(&rwlock->readers_cond);
        return -1;
    }

    // Инициализация счётчиков и флагов
    rwlock->active_readers = 0;
    rwlock->active_writer = 0;
    rwlock->waiting_readers = 0;
    rwlock->waiting_writers = 0;

    return 0;
}

int my_rwlock_destroy(my_rwlock_t *rwlock)
{
    if (rwlock == NULL)
    {
        return -1;
    }

    pthread_mutex_destroy(&rwlock->mutex);
    pthread_cond_destroy(&rwlock->readers_cond);
    pthread_cond_destroy(&rwlock->writers_cond);

    return 0;
}

int my_rwlock_rdlock(my_rwlock_t *rwlock)
{
    if (rwlock == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&rwlock->mutex);

    // Ожидание пока есть активный писатель или ожидающие писатели (приоритет писателям)
    rwlock->waiting_readers++;
    while (rwlock->active_writer || rwlock->waiting_writers > 0)
    {
        pthread_cond_wait(&rwlock->readers_cond, &rwlock->mutex);
    }
    rwlock->waiting_readers--;

    // Получение блокировки на чтение
    rwlock->active_readers++;

    pthread_mutex_unlock(&rwlock->mutex);

    return 0;
}

int my_rwlock_wrlock(my_rwlock_t *rwlock)
{
    if (rwlock == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&rwlock->mutex);

    // Ожидание пока есть активные читатели или активный писатель
    rwlock->waiting_writers++;
    while (rwlock->active_readers > 0 || rwlock->active_writer)
    {
        pthread_cond_wait(&rwlock->writers_cond, &rwlock->mutex);
    }
    rwlock->waiting_writers--;

    // Получение блокировки на запись
    rwlock->active_writer = 1;

    pthread_mutex_unlock(&rwlock->mutex);

    return 0;
}

int my_rwlock_unlock(my_rwlock_t *rwlock)
{
    if (rwlock == NULL)
    {
        return -1;
    }

    pthread_mutex_lock(&rwlock->mutex);

    // Освобождение блокировки на запись
    if (rwlock->active_writer)
    {
        rwlock->active_writer = 0;

        // Приоритет ожидающим писателям
        if (rwlock->waiting_writers > 0)
        {
            pthread_cond_signal(&rwlock->writers_cond);
        }
        else if (rwlock->waiting_readers > 0)
        {
            pthread_cond_broadcast(&rwlock->readers_cond);
        }
    }
    // Освобождение блокировки на чтение
    else if (rwlock->active_readers > 0)
    {
        rwlock->active_readers--;

        // Если больше нет активных читателей, будим ожидающего писателя
        if (rwlock->active_readers == 0 && rwlock->waiting_writers > 0)
        {
            pthread_cond_signal(&rwlock->writers_cond);
        }
    }

    pthread_mutex_unlock(&rwlock->mutex);

    return 0;
}
