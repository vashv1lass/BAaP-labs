#ifndef ALGORITHMS_H
#define ALGORITHMS_H 1

#include <stddef.h> // For size_t.

#include "auxiliary.h" // For compare_func_t and predicate_func_t.

// Performs a linear search for elements matching a target in a generic array.
void * linear_search(const void *, size_t, const void *, size_t, compare_func_t, size_t *);
// Performs a binary search for elements matching a target in a sorted generic array.
void * binary_search(const void *, size_t, const void *, size_t, compare_func_t, size_t *);
// Performs a predicate-based search for elements in a generic array.
void * predicate_search(const void *, size_t, size_t, size_t *, predicate_func_t, ...);

// Exchanges the values of two variables using their memory addresses.
static int swap(void *, void *, size_t);

// Performs quick sort on a generic array.
int quicksort(void *, size_t, size_t, compare_func_t);
// Performs selection sort on a generic array.
int selection_sort(void *, size_t, size_t, compare_func_t);
// Performs insertion sort on a generic array.
int insertion_sort(void *, size_t, size_t, compare_func_t);

#endif // ALGORITHMS_H