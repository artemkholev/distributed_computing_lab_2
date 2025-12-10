#!/bin/bash

# Benchmark script for comparing custom rwlock vs pthread_rwlock

echo "Benchmark: Comparing custom my_rwlock vs pthread_rwlock"
echo "=========================================================="
echo

# Test parameters
THREADS=(1 2 4 8)
INITIAL_KEYS=1000
TOTAL_OPS=100000
SEARCH_PERCENT=0.8
INSERT_PERCENT=0.1

echo "Test configuration:"
echo "  Initial keys: $INITIAL_KEYS"
echo "  Total operations: $TOTAL_OPS"
echo "  Search %: $SEARCH_PERCENT"
echo "  Insert %: $INSERT_PERCENT"
echo "  Delete %: $(echo "1.0 - $SEARCH_PERCENT - $INSERT_PERCENT" | bc)"
echo

for threads in "${THREADS[@]}"; do
    echo "================================================"
    echo "Testing with $threads thread(s)"
    echo "================================================"

    echo
    echo "--- Custom my_rwlock ---"
    echo -e "$INITIAL_KEYS\n$TOTAL_OPS\n$SEARCH_PERCENT\n$INSERT_PERCENT" | ./test_my_rwlock $threads

    echo
    echo "--- Standard pthread_rwlock ---"
    echo -e "$INITIAL_KEYS\n$TOTAL_OPS\n$SEARCH_PERCENT\n$INSERT_PERCENT" | ./test_pthread_rwlock $threads

    echo
done

echo
echo "Benchmark complete!"
