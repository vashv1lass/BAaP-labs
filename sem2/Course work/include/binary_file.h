/**
 * @file  binary_file.h
 * @brief Contains the basic functions (declarations) that may be needed when working with binary files.
 */

 #ifndef BINARY_FILE_H
 #define BINARY_FILE_H 1
 
 #include <stddef.h>  // For `size-t`.
 #include <stdio.h>   // For `FILE`.
 #include <stdbool.h> // For `bool`.
 
 #include "auxiliary.h" // For predicate_func_t.
 
 // Determines the size of a binary file by seeking to its end.
 static size_t binary_file_size(FILE *);
 
 // Checks if a binary file exists by attempting to open it.
 bool binary_file_exists(const char *);
 // Creates a binary file with optional overwrite control.
 int create_binary_file(const char *, bool);
 // Reads a binary file into dynamically allocated memory.
 void * read_binary_file(const char *, size_t *);
 // Appends binary data to a file.
 int append_to_binary_file(const char *, const void *, size_t);
 // Removes records from a binary file based on a predicate.
 int remove_from_binary_file(const char *, size_t, predicate_func_t, ...);
 
 #endif // BINARY_FILE_H