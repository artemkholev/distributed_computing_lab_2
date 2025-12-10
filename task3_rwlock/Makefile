CC = gcc
CFLAGS = -pthread -O3 -Wall
TARGETS = test_my_rwlock test_pthread_rwlock

all: $(TARGETS)

test_my_rwlock: test_my_rwlock.c my_rwlock.c my_rand.c
	$(CC) $(CFLAGS) -o test_my_rwlock test_my_rwlock.c my_rwlock.c my_rand.c

test_pthread_rwlock: test_pthread_rwlock.c my_rand.c
	$(CC) $(CFLAGS) -o test_pthread_rwlock test_pthread_rwlock.c my_rand.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
