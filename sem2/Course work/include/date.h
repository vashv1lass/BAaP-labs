/**
 * @file  date.h
 * @brief Contains the `date` structure declaration and date's function declarations.
 */

#ifndef DATE_H
#define DATE_H 1

#include <stdbool.h> // For `bool`.
#include <stddef.h>  // For `size_t`.
#include <stdio.h>   // For `FILE`.

/**
* @brief Structure representing a calendar date in Gregorian format.
*
* @details Stores day, month, and year components for date-related operations.
*          All fields should contain valid calendar values within typical ranges.
*/
typedef struct date {
int day;   /// Day of the month (1-31, exact range depends on month/year).
int month; /// Month number (1 = January, 12 = December).
int year;  /// Year (positive integer, typically 1900-2100 range, but can be other).
} date;

/// Maximum string buffer size for date in `DD.MM.YYYY` format (includes '\\n' and '\0' symbols).
#define DATE_STRING_BUFFER_SIZE ((size_t)12)

// Determines if a year is leap according to calendar rules.
static bool year_is_leap(int);

// Validates a date according to calendar rules.
bool date_is_valid(date);

// Converts a string in `DD.MM.YYYY` format to a `date` structure.
date str_to_date(const char *);
// Converts a date structure to a formatted string.
int date_to_str(char *, size_t, date);

// Reads and parses a date from an input stream into a `date` structure.
int fscan_date(FILE *, date *);
// Prints a formatted date to a file stream.
int fprint_date(FILE *, date);

/// Works same as `fscan_date()` but the input stream is `stdin`.
#define scan_date(d)  fscan_date(stdin, d)

// Compares two dates to determine their relative order.
int datecmp(date, date);

#endif // DATE_H