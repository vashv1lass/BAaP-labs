#ifndef BINARY_FILE_H
#define BINARY_FILE_H 1

#include <stddef.h>  // For size_t.
#include <stdio.h>   // For `FILE`.
#include <stdbool.h> // For bool.

#include "auxiliary.h" // For predicate_func_t.

// Determines the size of a binary file in bytes.
static size_t binary_file_size(FILE *);

// Creates a new empty binary file or overwrites an existing one.
int create_binary_file(const char *, bool);
// Reads the contents of a binary file into a dynamically allocated buffer.
void * read_binary_file(const char *, size_t *);
// Appends data to an existing binary file.
int append_to_binary_file(const char *, const void *, size_t);
// Removes data from a binary file based on a predicate function.
int remove_from_binary_file(const char *, size_t, predicate_func_t, ...);

#endif // BINARY_FILE_H