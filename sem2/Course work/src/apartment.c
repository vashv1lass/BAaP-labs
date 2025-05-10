/**
 * @file  apartment.c
 * @brief Contains the apartment's function definitions.
 */

#include "apartment.h"

#include <stdbool.h> // For `bool`, `true`, `false`.
#include <stdarg.h>  // For `va_list`, `va_copy()`, etc.
#include <stddef.h>  // For NULL.
#include <errno.h>   // For `errno` and its codes.
#include <limits.h>  // For `INT_MIN`.
#include <math.h>    // For `fabsf()`.
#include <string.h>  // For `strlen()`.
#include <stdio.h>   // For `FILE`.

#include "date.h"      // For `date`, `datecmp()`, etc.

/**
 * @brief      Validates an apartment structure's fields with strict criteria.
 *
 * @details    Checks if all apartment fields meet the following requirements:
 *             - `id` ≥ 0 (non-negative identifier).
 *             - `address` is a non-empty null-terminated string.
 *             - `rooms_count` > 0 (at least one room).
 *             - `area` >= `AREA_EPSILON` (1e-1) to avoid floating-point errors.
 *             - `floor` > 0 (valid floor number).
 *             - `cost` >= `COST_EPSILON` (1e-2) for monetary precision.
 *             - `addition_date` is valid (via `date_is_valid()`).
 *
 * @param[in]  apt Apartment structure to validate.
 *
 * @returns    `true` if all fields are valid, `false` otherwise.
 *
 * @note       - Uses `strlen()` to verify non-empty address strings.
 *             - Floating-point comparisons use `AREA_EPSILON` (1e-1) and `COST_EPSILON` (1e-2) tolerance.
 *             - Integer fields (`id`, `rooms_count`, `floor`) use direct range checks.
 *             - Depends on `date_is_valid()` for calendar date verification.
 */
bool apartment_is_valid(const apartment apt) {
    return apt.id >= 0 && strlen(apt.address) > 0 && apt.rooms_count > 0 && apt.area >= AREA_EPSILON &&
           apt.floor > 0 && apt.cost >= COST_EPSILON && date_is_valid(apt.addition_date);
}

/**
 * @brief      Prints apartment details to a file stream in localized format.
 *
 * @details    Validates input arguments, checks stream status, and outputs apartment data
 *             in Russian-localized format. Uses `fprintf()` for main fields and delegates
 *             date printing to `fprint_date()`. Handles errors from all I/O operations.
 *
 * @param[in]  output_stream Valid writable file stream (e.g., `stdout`, file pointer).
 * @param[in]  apt           Valid apartment structure to print.
 *
 * @errors     The function may fail and set `errno` to:
 *               - EINVAL: \p output_stream is NULL or \p apt is invalid.
 *               - EBADF:  Stream has errors (via `ferror()`).
 *               - Errors from `fprintf()` (e.g., EIO) or `fprint_date()`.
 *
 * @returns    0 on success.
 *             -1 on failure (invalid arguments, formatting error, or write failure).
 *
 * @note       - Output format (Russian labels):
 *                ```
 *                Идентификатор: %d\n
 *                Адрес: %s\n
 *                Количество комнат: %d\n
 *                Площадь: %.1f м^2\n
 *                Этаж: %d\n
 *                Стоимость: $%.2f\n
 *                Статус: %s\n
 *                Дата добавления в файл: [DD.MM.YYYY]
 *                ```
 *             - Floating-point formatting:
 *                - `area` with 1 decimal place (`%.1f`).
 *                - `cost` with 2 decimal places (`%.2f`).
 *             - Status translation: "Продана" (sold) or "Свободна" (available).
 *             - Depends on:
 *                - `apartment_is_valid()` for input validation.
 *                - `fprint_date()` for date formatting.
 *             - Encoding caveat: Assumes terminal/file supports Cyrillic characters.
 */
int fprint_apartment(FILE *output_stream, const apartment apt) {
    // Checking if the arguments are valid.
    if (output_stream == NULL || !apartment_is_valid(apt)) {
        // If not, terminating the function (setting `errno` to EINVAL, returning -1).
        errno = EINVAL;
        return -1;
    }
    
    // Checking if the output stream is valid.
    if (ferror(output_stream) != 0) {
        // If not, terminating the function (setting `errno` to EBADF, returning -1).
        errno = EBADF;
        return -1;
    }
    
    // Trying to print the apartment data (except the addition date).
    if (fprintf(output_stream,
                "Идентификатор: %d\nАдрес: %s\nКоличество комнат: %d\nПлощадь: %.1f м^2\nЭтаж: %d\nСтоимость: $%.2f\n"
                "Статус: %s\nДата добавления в файл: ", apt.id, apt.address, apt.rooms_count, apt.area,
                apt.floor, apt.cost, apt.sold ? "Продана" : "Свободна") < 0) {
        // If something went wrong, terminating the function (`errno` is already set, returning -1).
        return -1;
    }
    
    // Trying to print the addition date.
    if (fprint_date(output_stream, apt.addition_date) == -1) {
        // If failure, terminating the function (`errno` is already set, returning -1).
        return -1;
    }
    
    // Everything is ok, returning 0.
    return 0;
}

/**
 * @brief      Checks if an apartment's ID matches a specified target value.
 *
 * @details    This function validates the input apartment and extracts a target ID from a `va_list`
 *             to compare against the apartment's `id` field. Uses `va_copy` to safely handle variadic arguments.
 *
 * @param[in]  apt    Pointer to an `apartment` structure (must be valid and non-NULL).
 * @param[in]  args   Variadic argument list containing the target ID (type `int`).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: `apt` is NULL or points to an invalid apartment.
 *
 * @returns    - `true`: `apt->id` matches the target ID.
 *             - `false`: Mismatch, invalid `apt`, or missing/invalid arguments in `args`.
 *
 * @note       - Requires `args` to contain at least one `int` argument.
 *             - Does not validate the presence or type of arguments in `args`.
 */
bool id_match(const void *apt, va_list args) {
    // Checking the apartment pointer for validity.
    if (apt == NULL || !apartment_is_valid(*(const apartment *)apt)) {
        // If invalid, terminating the function (setting `errno` to EINVAL, returning false).
        errno = EINVAL;
        return false;
    }
    
    // Getting the target id via copying the variadic argument list.
    va_list args_copy;
    va_copy(args_copy, args);
    
    int target_id = va_arg(args_copy, int);
    
    va_end(args_copy);
    
    // Returning the comparison result.
    return ((const apartment *)apt)->id == target_id;
}

/**
 * @brief      Compares two apartment objects by their ID.
 *
 * @details    Validates pointer and structure integrity, then returns the difference between the `id` fields.
 *
 * @param[in]  apt_left   Pointer to the first apartment object (must not be NULL).
 * @param[in]  apt_right  Pointer to the second apartment object (must not be NULL).
 *
 * @errors     May set `errno` to:
 *               - EINVAL: `apt_left`/`apt_right` are NULL or structures are invalid (checked via `apartment_is_valid`).
 *
 * @returns    - Difference between the `id` values (`left.id - right.id`) for valid arguments.
 *               - `INT_MIN` on errors (e.g., invalid arguments).
 *
 * @note       - `INT_MIN` may conflict with legitimate data differences. Check `errno` after use.
 *             - Does not handle memory alignment — pointers must reference valid aligned structures.
 */
int apt_id_compare(const void *apt_left, const void *apt_right) {
    // Checking the validity of the passed arguments.
    if (apt_left == NULL || apt_right == NULL || !apartment_is_valid(*(const apartment *)apt_left) ||
        !apartment_is_valid(*(const apartment *)apt_right)) {
        // If invalid, terminating the function (setting errno to EINVAL, returning INT_MIN).
        errno = EINVAL;
        return INT_MIN;
    }
    
    // Returning the difference between the ID's.
    return ((const apartment *)apt_left)->id - ((const apartment *)apt_right)->id;
}

/**
 * @brief      Compares two apartments based on their `rooms_count` field.
 *
 * @details    Validates the input pointers and ensures both apartments are valid using `apartment_is_valid`.
 *             If valid, computes the difference between the `rooms_count` fields of the two apartments.
 *
 * @param[in]  apt_left   Pointer to the first apartment (must not be NULL and must be valid).
 * @param[in]  apt_right  Pointer to the second apartment (must not be NULL and must be valid).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers or invalid apartment data).
 *
 * @returns    - Difference between `rooms_count` values of `apt_left` and `apt_right` if inputs are valid.
 *               - INT_MIN: Error occurred (check `errno` for details).
 *
 * @note       - The function assumes `apartment_is_valid` correctly validates the apartment structure.
 *             - Thread-unsafe due to `errno` usage.
 */
int apt_rooms_count_compare(const void *apt_left, const void *apt_right) {
    // Checking the validity of the passed arguments.
    if (apt_left == NULL || apt_right == NULL || !apartment_is_valid(*(const apartment *)apt_left) ||
        !apartment_is_valid(*(const apartment *)apt_right)) {
        // If invalid, terminating the function (setting errno to EINVAL, returning INT_MIN).
        errno = EINVAL;
        return INT_MIN;
    }
    
    // Returning the difference between the `rooms_count`s.
    return ((const apartment *)apt_left)->rooms_count - ((const apartment *)apt_right)->rooms_count;
}

/**
 * @brief      Compares two apartments by area.
 *
 * @details    Computes the order of two apartments based on their area with a tolerance of `AREA_EPSILON`.
 *
 * @param[in]  apt_left   Pointer to the first apartment (must not be NULL, must be valid via `apartment_is_valid`).
 * @param[in]  apt_right  Pointer to the second apartment (must not be NULL, must be valid via `apartment_is_valid`).
 *
 * @errors     Sets `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers or invalid apartments).
 *
 * @returns    - -1: `apt_left->area` < `apt_right->area` (within tolerance).
 *             -  0: Areas are considered equal (difference < `AREA_EPSILON`).
 *             -  1: `apt_left->area` > `apt_right->area` (within tolerance).
 *             - `INT_MIN`: Error (check `errno`).
 *
 * @note       - Uses exact comparisons if the area difference exceeds the epsilon.
 *             - Thread-unsafe due to `errno` usage.
 */
int apt_area_compare(const void *apt_left, const void *apt_right) {
    // Checking the validity of the passed arguments.
    if (apt_left == NULL || apt_right == NULL || !apartment_is_valid(*(const apartment *)apt_left) ||
        !apartment_is_valid(*(const apartment *)apt_right)) {
        // If invalid, terminating the function (setting errno to EINVAL, returning INT_MIN).
        errno = EINVAL;
        return INT_MIN;
    }
    
    // Getting the difference between areas of left and right apartments.
    float area_diff = ((const apartment *)apt_left)->area - ((const apartment *)apt_right)->area;
    
    if (fabsf(area_diff) < AREA_EPSILON) {
        // If the difference is close to 0 (less than 0.1), then areas are equal.
        return 0;
    }
    
    // Returning -1 if the area of left apartment is less, 1 if greater.
    return area_diff < 0 ? -1 : 1;
}

/**
 * @brief      Compares two apartments based on their cost with floating-point tolerance.
 *
 * @details    Validates apartment structures, then compares their `cost` fields using a precision threshold (0.01).
 *             Returns -1/0/1 for sorting compatibility.
 *
 * @param[in]  apt_left   Pointer to the first apartment (must not be NULL).
 * @param[in]  apt_right  Pointer to the second apartment (must not be NULL).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers or invalid apartment structures).
 *
 * @returns    - -1:      `apt_left` cost is less than `apt_right` (difference > `COST_EPSILON`).
 *             - 0:       Costs are equal (difference ≤ `COST_EPSILON`).
 *             - 1:       `apt_left` cost is greater than `apt_right` (difference > `COST_EPSILON`).
 *             - INT_MIN: Validation error (check `errno`).
 *
 * @note       - Uses `COST_EPSILON = 0.01` to handle floating-point imprecision.
 *             - `INT_MIN` indicates errors, but may overlap with extreme valid values.
 */
int apt_cost_compare(const void *apt_left, const void *apt_right) {
    // Checking the validity of the passed arguments.
    if (apt_left == NULL || apt_right == NULL || !apartment_is_valid(*(const apartment *)apt_left) ||
        !apartment_is_valid(*(const apartment *)apt_right)) {
        // If invalid, terminating the function (setting errno to EINVAL, returning INT_MIN).
        errno = EINVAL;
        return INT_MIN;
    }
    
    // Getting the difference between costs of left and right apartments.
    float cost_diff = ((const apartment *)apt_left)->cost - ((const apartment *)apt_right)->cost;
    
    if (fabsf(cost_diff) < COST_EPSILON) {
        // If the difference is close to 0 (less than 0.01), then costs are equal.
        return 0;
    }
    
    // Returning -1 if the cost of left apartment is less, 1 if greater.
    return cost_diff < 0 ? -1 : 1;
}

/**
 * @brief      Compares two apartments based on their `addition_date` field.
 *
 * @details    Validates the input pointers and ensures both apartments are valid using `apartment_is_valid`.
 *             If valid, compares the `addition_date` fields of the two apartments using `datecmp`.
 *
 * @param[in]  apt_left   Pointer to the first apartment (must not be NULL and must be valid).
 * @param[in]  apt_right  Pointer to the second apartment (must not be NULL and must be valid).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers or invalid apartment data).
 *
 * @returns    - Negative value if `apt_left->addition_date` is earlier than `apt_right->addition_date`.
 *               - Zero if `apt_left->addition_date` is equal to `apt_right->addition_date`.
 *               - Positive value if `apt_left->addition_date` is later than `apt_right->addition_date`.
 *               - INT_MIN: Error occurred (check `errno` for details).
 *
 * @note       - Assumes `apartment_is_valid` correctly validates the apartment structure.
 *             - Thread-unsafe due to `errno` usage.
 */
int apt_addition_date_compare(const void *apt_left, const void *apt_right) {
    // Checking the validity of the passed arguments.
    if (apt_left == NULL || apt_right == NULL || !apartment_is_valid(*(const apartment *)apt_left) ||
        !apartment_is_valid(*(const apartment *)apt_right)) {
        // If invalid, terminating the function (setting errno to EINVAL, returning INT_MIN).
        errno = EINVAL;
        return INT_MIN;
    }
    
    return datecmp(((const apartment *)apt_left)->addition_date, ((const apartment *)apt_right)->addition_date);
}