/**
 * @file  apartment.h
 * @brief Contains the `apartment` structure declaration and apartment's function declarations.
 */

#ifndef APARTMENT_H
#define APARTMENT_H 1

#include <stdbool.h> // For `bool`.
#include <stdarg.h>  // For `va_list`.
#include <stdio.h>   // For `FILE`.

#include "auxiliary.h" // For `STRING_BUFFER_MAX_SIZE`.
#include "date.h"      // For `date`.

/// Minimum value by which the cost can differ.
#define COST_EPSILON           ((float)1e-2)
/// Minimum value by which the area can differ.
#define AREA_EPSILON           ((float)1e-1)

/**
 * @brief Structure representing a real estate apartment entry.
 *
 * @details This structure stores comprehensive information about an apartment listing,
 * including identification, location, characteristics, financial data, and status.
 * All numerical fields (except boolean) must contain non-negative values.
 */
typedef struct apartment {
    /// Unique identifier of the apartment (non-negative integer, positive integer when in file).
    int id;

    /// Full address string (null-terminated, max length STRING_BUFFER_MAX_SIZE-1).
    char address[STRING_BUFFER_MAX_SIZE];

    /// Number of rooms in the apartment (positive integer).
    int rooms_count;
    /// Total area in square meters (positive value).
    float area;
    /// Floor number where the apartment is located (positive integer).
    int floor;

    /// Listing price in dollars (non-negative value).
    float cost;

    /// Sales status (`true` - apartment is sold).
    bool sold;

    /// Date when the apartment was added to the system.
    date addition_date;
} apartment;

// Validates an apartment structure's fields with strict criteria.
bool apartment_is_valid(apartment);

// Prints apartment details to a file stream in localized format.
int fprint_apartment(FILE *, apartment);

/// Works same as `fprint_apartment()`, but output stream is `stdout`.
#define print_apartment(apt) fprint_apartment(stdout, apt)

// Checks if an apartment's ID matches a specified target value.
bool id_match(const void *, va_list);
// Compares two apartment objects by their ID.
int apt_id_compare(const void *, const void *);
// Compares two apartments based on their `rooms_count` field.
int apt_rooms_count_compare(const void *, const void *);
// Compares two apartments by area.
int apt_area_compare(const void *, const void *);
// Compares two apartments based on their cost with floating-point tolerance.
int apt_cost_compare(const void *, const void *);
// Compares two apartments based on their `addition_date` field.
int apt_addition_date_compare(const void *, const void *);

#endif // APARTMENT_H