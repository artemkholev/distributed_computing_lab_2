#ifndef MY_RWLOCK_H
#define MY_RWLOCK_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_cond;
    pthread_cond_t writers_cond;
    int active_readers;        // Number of currently reading threads
    int active_writer;         // Flag: 1 if writer is active, 0 otherwise
    int waiting_readers;       // Number of threads waiting to read
    int waiting_writers;       // Number of threads waiting to write
} my_rwlock_t;

// Initialize the rwlock
int my_rwlock_init(my_rwlock_t *rwlock);

// Destroy the rwlock
int my_rwlock_destroy(my_rwlock_t *rwlock);

// Acquire read lock
int my_rwlock_rdlock(my_rwlock_t *rwlock);

// Acquire write lock
int my_rwlock_wrlock(my_rwlock_t *rwlock);

// Release lock (both read and write)
int my_rwlock_unlock(my_rwlock_t *rwlock);

#endif // MY_RWLOCK_H
