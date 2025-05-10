/**
 * @file  database.h
 * @brief Contains the functions (declarations) that may be needed when working with apartments in the context of binary
 *        file.
 */

#ifndef DATABASE_H
#define DATABASE_H 1

#include <stddef.h> // For `size_t`.
#include <stdbool.h> // For `bool`.

#include "apartment.h" // For `apartment`.

// Reads an array of apartment structures from a binary file.
apartment * read_apartments_from_binary_file(const char *, size_t *);

// Finds an apartment by ID in a binary file and returns its details.
apartment get_apartment_by_id(const char *, int);

// Checks if an apartment ID is unused in a binary file.
bool apartment_id_is_unused(const char *, int);

// Generates a unique apartment ID based on existing records in a binary file.
int generate_unique_apartment_id(const char *);

#endif // DATABASE_H