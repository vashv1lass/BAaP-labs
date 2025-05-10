/**
 * @file  algorithms.h
 * @brief Contains the basic functions (declarations) that may be needed when writing the necessary algorithms.
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H 1

#include <stddef.h> // For `size_t`.

#include "auxiliary.h" // For `compare_func_t`, `predicate_func_t`.

// Performs a linear search for elements in an array matching a target value.
void * linear_search(const void *, size_t, const void *, size_t, compare_func_t, size_t *);
// Performs a binary search for elements in a sorted array matching a target value.
void * binary_search(const void *, size_t, const void *, size_t, compare_func_t, size_t *);
// Searches an array for elements matching a predicate and returns a dynamically allocated array of matches.
void * predicate_search(const void *, size_t, size_t, size_t *, predicate_func_t, ...);

// Swaps the contents of two memory blocks of a specified type.
static int swap(void *, void *, size_t);

// Sorts an array using the quicksort algorithm.
int quicksort(void *, size_t, size_t, compare_func_t);
// Sorts an array using selection sort algorithm.
int selection_sort(void *, size_t, size_t, compare_func_t);
// Sorts an array using insertion sort algorithm.
int insertion_sort(void *, size_t, size_t, compare_func_t);

#endif // ALGORITHMS_H