/**
 * @file  date.c
 * @brief Contains the date's function definitions.
 */

 #include "date.h"

 #include <stdbool.h> // For `bool`, `false`.
 #include <errno.h>   // For `errno` and its codes.
 #include <stddef.h>  // For `NULL`, `size_t`, etc.
 #include <string.h>  // For `strlen()`.
 #include <stdio.h>   // For `FILE`, `ferror()`, etc.
 #include <limits.h>  // For `INT_MIN`.
 
 #include "safeio.h"  // For `safe_fgets()`, `safe_sscanf()`, etc.
 
 /**
  * @brief      Determines if a year is leap according to calendar rules.
  *
  * @details    Checks leap year status using Julian calendar for years < 1582
  *             and Gregorian calendar for years ≥ 1582. Handles invalid input
  *             by setting `errno`.
  *
  * @param[in]  year Year to check (must be ≥ 1).
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: \p year is ≤ 0.
  *
  * @returns    `true` if leap year, `false` otherwise (including invalid years).
  *
  * @note       - Uses historical calendar transition in 1582.
  *             - Julian rule: year divisible by 4.
  *             - Gregorian rule: (divisible by 4 & not 100) or divisible by 400.
  */
 static bool year_is_leap(int year) {
     // Checking if the argument is valid.
     if (year <= 0) {
         // If not, terminating the function (setting `errno` to EINVAL and returning -1).
         errno = EINVAL;
         return false;
     }
     
     // Returning true if the day is leap, false otherwise.
     if (year < 1582) {
         // Julian calendar.
         return year % 4 == 0;
     }
     // Gregorian calendar.
     return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
 }
 
 /**
  * @brief      Validates a date according to calendar rules.
  *
  * @details    Checks if the date is valid by verifying:
  *             - Year > 0, month ∈ [1, 12], day > 0.
  *             - Day does not exceed the maximum for the month (adjusted for leap years).
  *             Uses `year_is_leap()` to handle February in leap years.
  *
  * @param[in]  d Date to validate.
  *
  * @returns    `true` if the date is valid, `false` otherwise.
  *
  * @note       - Depends on `year_is_leap()` for February validation.
  *             - Months are 1-based (January = 1).
  *             - Does not check for `errno` (no system errors generated).
  */
 bool date_is_valid(const date d) {
     // Number of days in each month.
     int days_count[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
     
     // If something is not positive or the month is more than 12, returning false.
     if (d.year <= 0 || (d.month <= 0 || d.month > 12) || d.day <= 0) {
         return false;
     }
     
     // If the year is leap, February has one more day than usual.
     if (year_is_leap(d.year)) {
         days_count[1]++;
     }
     
     // Returning true if the day is corresponding to the month, false otherwise.
     return d.day <= days_count[d.month - 1];
 }
 
 /**
  * @brief      Converts a string in `DD.MM.YYYY` format to a `date` structure.
  *
  * @details    Parses a date string with strict format validation. The function performs:
  *               1. NULL argument check,
  *               2. Format validation (10 characters, `.` separators),
  *               3. Numeric parsing via `safe_sscanf()`,
  *               4. Logical date validation (valid day/month/year ranges, leap years).
  *             All errors result in `errno = EINVAL` and a `{ -1, -1, -1 }` return value.
  *
  * @param[in]  date_str  The input string (must follow `DD.MM.YYYY` format and not be NULL).
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: Invalid argument (`NULL`), malformed format, parsing failure, or invalid date.
  *
  * @returns    On success: A valid `date` structure with `day`, `month`, `year` ≥ 1.
  *             On failure: `{ -1, -1, -1 }` with `errno` set to EINVAL.
  *
  * @note       - Strict format: Requires exactly two digits for day/month and four for year (e.g., "01.02.2023").
  *             - Relies on `date_is_valid()` to check logical date correctness (e.g., February 30 is invalid).
  *             - Not thread-safe if `date_is_valid()` or `safe_sscanf()` use shared state.
  *             - Partial parsing (e.g., "12.05." without year) is rejected.
  */
 date str_to_date(const char *date_str) {
     // Default value for `date`.
     date d = { -1, -1, -1 };
     
     // Checking if the argument (date_str) is valid.
     if (date_str == NULL) {
         // If not, terminating the function (setting `errno` to EINVAL and returning `d = { -1, -1, -1 }`.
         errno = EINVAL;
         return d;
     }
     
     // Checking if the date format (DD.MM.YYYY) is valid.
     if (strlen(date_str) != 10 || date_str[2] != '.' || date_str[5] != '.') {
         // If not, terminating the function (setting `errno` to EINVAL and returning `d = { -1, -1, -1 }`.
         errno = EINVAL;
         return d;
     }
     
     // Trying to parse the string to `date` type.
     if (safe_sscanf(date_str, "%d.%d.%d", &d.day, &d.month, &d.year) != 3) {
         // If failure, setting `errno` to EINVAL and `d` to { -1, -1, -1 }.
         errno = EINVAL;
         d.day = d.month = d.year = -1;
     }
     
     // Checking if the result date is valid.
     if (!date_is_valid(d)) {
         // If not, setting errno to EINVAL and `d` to { -1, -1, -1 }.
         errno = EINVAL;
         d.day = d.month = d.year = -1;
     }
     
     // Returning `d`.
     return d;
 }
 
 /**
  * @brief      Converts a date structure to a formatted string.
  *
  * @details    Validates input arguments, checks date correctness via `date_is_valid()`,
  *             and formats the date into "DD.MM.YYYY" using `snprintf()`. Ensures buffer
  *             safety by verifying `dest_buffer_size`.
  *
  * @param[out] dest               Destination buffer (must not be NULL).
  * @param[in]  dest_buffer_size   Buffer size (must be ≥ `DATE_STRING_BUFFER_SIZE`).
  * @param[in]  d                  Valid date structure to convert.
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: \p dest is NULL, buffer too small, or \p d is invalid.
  *
  * @returns    0 on success.
  *             -1 on failure (invalid arguments, formatting error, or buffer overflow).
  *
  * @note       - Format follows "DD.MM.YYYY" pattern (e.g., "31.12.2025").
  *             - Depends on `date_is_valid()` for date verification.
  *             - Requires `DATE_STRING_BUFFER_SIZE` ≥ 11 (for 10 characters + null terminator).
  */
 int date_to_str(char *dest, size_t dest_buffer_size, const date d) {
     // Checking if the arguments are valid.
     if (dest == NULL || dest_buffer_size < DATE_STRING_BUFFER_SIZE || !date_is_valid(d)) {
         // If not, terminating the function (setting `errno` to EINVAL and returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Converting the date to a string in the DD.MM.YYYY format.
     int conversion_result = snprintf(dest, dest_buffer_size, "%02d.%02d.%04d", d.day, d.month, d.year);
     
     // If the conversion was successful, returning 0, otherwise -1.
     return conversion_result >= 0 && conversion_result < dest_buffer_size ? 0 : -1;
 }
 
 /**
  * @brief      Reads and parses a date from an input stream into a `date` structure.
  *
  * @details    This function reads a line from `input_stream` via `safe_fgets()`, then parses it into
  *             a `date` structure using `str_to_date()`. It validates arguments, checks stream errors,
  *             and ensures the parsed date is logically valid (via `date_is_valid()`).
  *
  * @param[in]  input_stream The input stream to read from (must not be NULL).
  * @param[out] d            Pointer to a `date` structure to store the result (must not be NULL).
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: Invalid arguments (`NULL` pointers), malformed date format, or invalid date.
  *               - EBADF: The stream had an existing error state before the call.
  *               - Other errors: See `safe_fgets()` documentation.
  *
  * @returns    - `1`: Success (valid date parsed).
  *             - `0`: Invalid date (format or logical error, `errno` set to EINVAL).
  *             - `EOF`: Input/output error or end-of-file reached.
  *
  * @note       - Relies on `safe_fgets()` to read the input line (buffer size: `DATE_STRING_BUFFER_SIZE`).
  *             - The input must follow the `DD.MM.YYYY` format (strict parsing).
  *             - Not thread-safe if `safe_fgets()` or `date_is_valid()` use shared state.
  *             - Partial reads (e.g., truncated lines) may result in parsing failures.
  */
 int fscan_date(FILE *input_stream, date *d) {
     // Checking if the arguments are valid.
     if (input_stream == NULL || d == NULL) {
         // If not, terminating the function (setting `errno` to EINVAL and returning EOF).
         errno = EINVAL;
         return EOF;
     }
     
     // Checking the validity of input stream.
     if (ferror(input_stream) != 0) {
         // If input stream is invalid, terminating the function (setting `errno` to EBADF, returning EOF).
         errno = EBADF;
         return EOF;
     }
     
     // Trying to read the date to a string buffer.
     char d_str[DATE_STRING_BUFFER_SIZE];
     if (safe_fgets(d_str, DATE_STRING_BUFFER_SIZE, input_stream) == NULL) {
         /*
          * If error, terminating the function
          * (`errno` is already set, returning EOF).
          */
         return EOF;
     }
     
     // Trying to parse the string to the `date` type.
     *d = str_to_date(d_str);
     if (!date_is_valid(*d)) {
         // If failure, terminating the function (`errno` is already set, returning 0).
         return 0;
     }
     
     // Everything is ok, returning 1.
     return 1;
 }
 
 /**
  * @brief      Prints a formatted date to a file stream.
  *
  * @details    Validates the output stream and date, converts the date to "DD.MM.YYYY"
  *             format using `date_to_str()`, and writes it to the stream. Handles
  *             errors from all validation/conversion steps.
  *
  * @param[in]  output_stream Valid writable file stream (e.g., `stdout`, file pointer).
  * @param[in]  d             Valid date structure to print.
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: \p output_stream is NULL or \p d is invalid.
  *               - EBADF:  Stream has errors (via `ferror()`).
  *               - Errors from `date_to_str()` (e.g., ENOMEM) or `fputs()` (e.g., EIO).
  *
  * @returns    0 on success.
  *             -1 on failure (invalid arguments, conversion error, or write failure).
  *
  * @note       - Format follows "DD.MM.YYYY" pattern (e.g., "01.03.2025").
  *             - Does NOT append a newline character.
  *             - Depends on `date_to_str()` for formatting.
  *             - Stream must be opened in a mode compatible with text writing.
  */
 int fprint_date(FILE *output_stream, const date d) {
     // Checking if the arguments are valid.
     if (output_stream == NULL || !date_is_valid(d)) {
         // If not, terminating the function (setting `errno` to EINVAL and returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Checking if the output stream is valid.
     if (ferror(output_stream) != 0) {
         // If not, terminating the function (setting `errno` to EBADF and returning -1).
         errno = EBADF;
         return -1;
     }
     
     // Trying to prepare the date for printing.
     char d_str[DATE_STRING_BUFFER_SIZE];
     if (date_to_str(d_str, DATE_STRING_BUFFER_SIZE, d) == -1) {
         // If failed, terminating the function (`errno` is already set, returning -1).
         return -1;
     }
     
     // Trying to print the date.
     if (fputs(d_str, output_stream) == EOF) {
         // If failure, terminating the function (`errno` is already set, returning -1).
         return -1;
     }
     
     // Everything is ok, returning 0.
     return 0;
 }
 
 /**
  * @brief      Compares two dates to determine their relative order.
  *
  * @details    Validates the input dates using `date_is_valid` and compares them field by field
  *             in the order of year, month, and day. Returns an integer indicating whether the left date is
  *             earlier, equal to, or later than the right date.
  *
  * @param[in]  left   The first date to compare (must be valid).
  * @param[in]  right  The second date to compare (must be valid).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid arguments (either `left` or `right` is not a valid date).
  *
  * @returns    - Negative value if `left` is earlier than `right`.
  *               - Zero if `left` is equal to `right`.
  *               - Positive value if `left` is later than `right`.
  *               - INT_MIN: Error occurred (check `errno` for details).
  *
  * @note       - Assumes `date_is_valid` correctly validates the date structure.
  *             - Thread-unsafe due to `errno` usage.
  */
 int datecmp(const date left, const date right) {
     // Checking if the arguments are valid.
     if (!date_is_valid(left) || !date_is_valid(right)) {
         // If invalid, terminating the function (setting errno to EINVAL, returning INT_MIN).
         errno = EINVAL;
         return INT_MIN;
     }
     
     // Comparing year.
     if (left.year != right.year) {
         return left.year < right.year ? -1 : 1;
     }
     
     // Comparing month.
     if (left.month != right.month) {
         return left.month < right.month ? -1 : 1;
     }
     
     // Comparing day.
     if (left.day != right.day) {
         return left.day < right.day ? -1 : 1;
     }
     
     // Dates are equal.
     return 0;
 }