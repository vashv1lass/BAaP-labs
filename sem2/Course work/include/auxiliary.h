#ifndef AUXILIARY_H
#define AUXILIARY_H 1

#include <stdbool.h> // For bool.
#include <stdarg.h>  // For va_list.

/**
 * @details Alias for a comparison function that defines the order of two elements.
 *          The function takes two pointers to data (const void *) and returns an integer indicating their order:
 *            - Negative if the first argument < second
 *            - Zero if arguments are equal
 *            - Positive if the first argument > second
 */
typedef int (*compare_func_t)(const void *, const void *);
/**
 * @details Alias for a predicate function.
 *          The function takes a pointer to data (const void *) and a variable argument list (va_list).
 *          Returns a boolean (bool) indicating if the element meets the condition.
 */
typedef bool (*predicate_func_t)(const void*, va_list);

#endif // AUXILIARY_H