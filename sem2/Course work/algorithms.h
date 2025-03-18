#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <stddef.h>  // size_t
#include <stdbool.h> // bool
#include <stdarg.h>  // va_list

typedef int (*compare_func_t)(const void *, const void *);

// Searches for the specified element in an array using a linear search.
void * linear_search(const void *, size_t, const void *, size_t, compare_func_t, size_t *);
// Searches for the specified element in sorted array using a binary search.
void * binary_search(const void *, size_t, const void *, size_t, compare_func_t, size_t *);

// Swaps the values of two variables.
static void swap(void *, void *, size_t);

// Sorts the array according to the passed comparison function (using the quick sort algorithm).
void quicksort(void *, size_t, size_t, compare_func_t);
// Sorts the array according to the passed comparison function (using the selection sort algorithm).
void selection_sort(void *, size_t, size_t, compare_func_t);
// Sorts the array according to the passed comparison function (using the insertion sort algorithm).
void insertion_sort(void *, size_t, size_t, compare_func_t);

typedef bool (*predicate_func_t)(const void *, va_list);

// Searches for all the elements that satisfy the condition.
void * predicate_search(const void *, size_t, size_t, size_t *, predicate_func_t, ...);

#endif // ALGORITHMS_H