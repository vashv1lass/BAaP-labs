/**
 * @file  algorithms.c
 * @brief Contains the basic functions (definitions) that may be needed when writing the necessary algorithms.
 */

#include "algorithms.h"

#include <stddef.h> // For `size_t`
#include <errno.h>  // For `errno` and errno codes.
#include <stdint.h> // For `uintptr_t`.
#include <stdlib.h> // For `calloc()`, `realloc()`, etc.
#include <memory.h> // For `memcpy()`.
#include <stdarg.h> // For `va_list`, `va_copy()`, etc.
#include <stdbool.h> // For `bool`, `true`, `false`.

#include "auxiliary.h" // For `compare_func_t`, `predicate_func_t`.

/**
 * @brief      Performs a linear search for elements in an array matching a target value.
 *
 * @details    Searches for all occurrences of `target_element` in `array` using the `compare` function.
 *             Returns a dynamically allocated array of matches, with the count stored in `found_size`.
 *
 * @param[in]  array           Pointer to the start of the array (must not be NULL).
 * @param[in]  size            Number of elements in the array.
 * @param[in]  target_element  Pointer to the element used for comparison (must not be NULL).
 * @param[in]  type_size       Size of one array element in bytes (must be >0).
 * @param[in]  compare         Comparison function returning 0 for matching elements (must not be NULL).
 * @param[out] found_size      Pointer to store the number of matches (must not be NULL).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers or `type_size == 0`).
 *               - EOVERFLOW: Overflow in `size * type_size` calculation (exceeds `SIZE_MAX`).
 *               - ENOMEM: Memory allocation failure.
 *               - Other codes: Errors set by the `compare` function.
 *
 * @returns    - Pointer to the array of matches (free with `free()`).
 *               - NULL: Error or no matches (check `*found_size` and `errno`).
 *
 * @note       - Uses `realloc()` for dynamic result buffer expansion, which may be inefficient for large datasets.
 *             - `found_size` is set to `(size_t)-1` on errors.
 *             - The `compare` function must handle `errno` correctly to avoid false errors.
 */
void * linear_search(const void *array, size_t size, const void *target_element, size_t type_size,
                     compare_func_t compare, size_t *found_size) {
    // Checking the validity of passed arguments.
    if (array == NULL || target_element == NULL || type_size == 0 || compare == NULL || found_size == NULL) {
        // If invalid, terminating function with error report.
        errno = EINVAL;
        
        if (found_size != NULL) {
            *found_size = (size_t)-1;
        }
        
        return NULL;
    }
    
    // Checking if the total array size does not exceed the system limits (SIZE_MAX).
    if (size > SIZE_MAX / type_size) {
        /*
         * If exceeds, interrupting the function
         * (setting errno to EOVERFLOW, assigning to found_size the special "undefined" value `(size_t)-1`
         * and returning NULL).
         */
        errno = EOVERFLOW;
        *found_size = (size_t)-1;
        return NULL;
    }
    
    // Initializing array where the found elements will be written and its size.
    size_t found_capacity = 16;
    *found_size = 0;
    void *found = malloc(found_capacity * type_size);
    
    // Checking the success of memory allocation.
    if (found == NULL) {
        // If failed, terminating the function with error report.
        errno = ENOMEM;
        *found_size = (size_t)-1;
        return NULL;
    }
    
    for (size_t i = 0; i < size; i++) {
        // Getting the address of the i-th element.
        const void *current_element = (const void *)((const uintptr_t)array + i * type_size);
        
        // Getting the comparison result.
        int saved_errno = errno;
        int compare_result = compare(current_element, target_element);
        if (saved_errno != errno) {
            // If comparison failure, terminating the function (errno is set, returning NULL).
            free(found);
            *found_size = (size_t)-1;
            return NULL;
        }
        
        if (compare_result == 0) {
            // Resizing the results array to accommodate the new match if necessary.
            if (*found_size >= found_capacity) {
                found_capacity *= 2;
                void *new_found = realloc(found, found_capacity * type_size);
                if (new_found == NULL) {
                    /*
                     * If the memory reallocation was failed, interrupting the function
                     * (setting errno to ENOMEM, clearing the buffer of found elements,
                     * assigning to found_size the special "undefined" value `(size_t)-1` and returning NULL).
                     */
                    free(found);
                    errno = ENOMEM;
                    *found_size = (size_t)-1;
                    return NULL;
                }
                found = new_found;
            }
            
            // Getting the current element position in the array of found elements.
            void *current_found_element = (void *)((uintptr_t)found + *found_size * type_size);
            
            // Copying current element to the array of found elements.
            memcpy(current_found_element, current_element, type_size);
            
            // Increasing the found elements counter.
            ++*found_size;
        }
    }
    
    // Truncating the found elements array to its real size.
    if (*found_size == 0) {
        free(found);
        found = NULL;
    } else {
        void *trimmed = realloc(found, *found_size * type_size);
        if (trimmed != NULL) {
            found = trimmed;
        }
    }
    
    // Everything is ok, returning the array of found elements.
    return found;
}

/**
 * @brief      Performs a binary search for elements in a sorted array matching a target value.
 *
 * @details    Searches for all contiguous occurrences of `target_element` in a sorted `array` using the `compare`
 *             function. Returns a dynamically allocated array of matches, with the count stored in `found_size`.
 *             The array must be sorted in ascending order according to `compare`.
 *
 * @param[in]  array           Pointer to the start of the sorted array (must not be NULL).
 * @param[in]  size            Number of elements in the array.
 * @param[in]  target_element  Pointer to the element used for comparison (must not be NULL).
 * @param[in]  type_size       Size of one array element in bytes (must be >0).
 * @param[in]  compare         Comparison function returning <0, 0, or >0 for less-than, equal-to, or greater-than cases
 *                             (must not be NULL).
 * @param[out] found_size      Pointer to store the number of matches or error status (must not be NULL).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers, `type_size == 0`, or invalid `compare` result).
 *               - EOVERFLOW: Overflow in `size * type_size` calculation (exceeds `SIZE_MAX`).
 *               - ENOMEM: Memory allocation failure during `calloc`.
 *               - Other codes: Errors set by the `compare` function.
 *
 * @returns    - Pointer to the array of matches (free with `free()`).
 *             - NULL: Error or no matches (check `*found_size` and `errno`).
 *
 * @note       - Uses `malloc()` for dynamic memory allocation of the result array.
 *             - `found_size` is set to `(size_t)-1` on errors, `0` if no matches are found.
 *             - Assumes the input array is sorted; behavior is undefined otherwise.
 *             - Thread-unsafe due to `errno` usage.
 */
void * binary_search(const void *array, size_t size, const void *target_element, size_t type_size,
                     compare_func_t compare, size_t *found_size) {
    // Checking if the arguments (array, target_element, type_size, compare and found_size) are valid.
    if (array == NULL || target_element == NULL || type_size == 0 || compare == NULL || found_size == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning NULL).
        errno = EINVAL;
        
        if (found_size != NULL) {
            // If found_size is not NULL, assigning to found_size the special "undefined" value `(size_t)-1`.
            *found_size = (size_t)-1;
        }
        
        return NULL;
    }
    
    // Checking if the total array size does not exceed the system limits (SIZE_MAX).
    if (size > SIZE_MAX / type_size) {
        /*
         * If exceeds, interrupting the function
         * (setting errno to EOVERFLOW, and returning NULL).
         */
        errno = EOVERFLOW;
        *found_size = (size_t)-1;
        return NULL;
    }
    
    // Initializing array where the found elements will be written and its size (for now, it is undefined).
    void *found = NULL;
    *found_size = 0;
    
    // Checking if the size is greater than 0.
    if (size == 0) {
        // If not, interrupting the function (it's not an error, just a special case).
        return NULL;
    }
    
    // Initializing the binary search bounds and the index of found element.
    size_t left = 0;
    size_t right = size - 1;
    size_t found_idx = (size_t)-1;
    
    // Finding the index of the one of target elements.
    while (left <= right) {
        // Getting the middle element index.
        size_t middle = left + (right - left) / 2;
        
        // Getting the address of the middle element.
        const void *middle_element = (const void *)((const uintptr_t)array + middle * type_size);
        
        // Getting the result of comparison of middle and target elements.
        int saved_errno = errno;
        int compare_result = compare(middle_element, target_element);
        if (saved_errno != errno) {
            // If the comparison was failed, terminating the function (errno is set, returning NULL).
            return NULL;
        }
        
        if (compare_result < 0) {
            // If the middle element is less than target element, then left bound needs to be set to middle.
            left = middle + 1;
        } else if (compare_result > 0) {
            // If greater, then right bound needs to be set to middle.
            if (middle == 0) {
                break;
            }
            
            right = middle - 1;
        } else {
            // If equal, we found it!
            found_idx = middle;
            break;
        }
    }
    
    // Checking if the target element was found.
    if (found_idx != (size_t)-1) {
        // If found, finding the index leftmost element that is equal to target element.
        size_t low = 0;
        size_t high = found_idx;
        while (low < high) {
            // Getting the middle element position.
            size_t mid = low + (high - low) / 2;
            
            // Getting the middle element address.
            const void *mid_element = (const void *)((const uintptr_t)array + mid * type_size);
            
            // Getting the result of comparison of middle and target elements.
            int saved_errno = errno;
            int compare_result = compare(mid_element, target_element);
            if (saved_errno != errno) {
                // If the comparison was failed, terminating the function (errno is set, returning NULL).
                return NULL;
            }
            
            if (compare_result < 0) {
                low = mid + 1;
            } else {
                high = mid;
            }
        }
        size_t leftmost_idx = low;
        
        // Finding the index of rightmost element that is equal to target element.
        low = found_idx, high = size - 1;
        while (low < high) {
            size_t mid = low + (high - low + 1) / 2;
            const void *mid_element = (const void *)((uintptr_t)array + mid * type_size);
            
            // Getting the result of comparison of middle and target elements.
            int saved_errno = errno;
            int compare_result = compare(mid_element, target_element);
            if (saved_errno != errno) {
                // If the comparison was failed, terminating the function (errno is set, returning NULL).
                return NULL;
            }
            
            if (compare_result > 0) {
                high = mid - 1;
            } else {
                low = mid;
            }
        }
        size_t rightmost_idx = low;
        
        // Trying to allocate memory for the found elements.
        found = malloc((rightmost_idx - leftmost_idx + 1) * type_size);
        if (found == NULL) {
            /*
             * If failure, interrupting the function
             * (setting errno to ENOMEM, assigning to found_size the special "undefined" value `(size_t)-1`
             * and returning NULL).
             */
            errno = ENOMEM;
            return NULL;
        }
        
        // Setting the size of the found elements.
        *found_size = rightmost_idx - leftmost_idx + 1;
        
        memcpy(found, (const void *)((const uintptr_t)array + leftmost_idx * type_size), *found_size * type_size);
    }
    
    // Everything is ok, returning the found elements array.
    return found;
}

/**
 * @brief      Searches an array for elements matching a predicate and returns a dynamically allocated array of matches.
 *
 * @details    This function iterates over an array, applies a variadic predicate function to each element,
 *             and collects all elements that satisfy the predicate. It handles memory allocation, overflow checks,
 *             and ensures thread safety for predicate arguments via `va_copy`.
 *
 * @param[in]  array        The input array (must not be NULL).
 * @param[in]  size         Number of elements in the array.
 * @param[in]  type_size    Size of each element in bytes (must be >0).
 * @param[out] found_size   Pointer to store the number of matched elements (must not be NULL).
 *                          Set to `(size_t)-1` on error or overflow.
 * @param[in]  predicate    Predicate function (must not be NULL).
 *                          Signature: `bool predicate(const void *element, va_list args)`.
 * @param[in]  ...          Variadic arguments passed to the predicate.
 *
 * @errors     The function may fail and set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers, zero type_size).
 *               - EOVERFLOW: `size * type_size` exceeds `SIZE_MAX`.
 *               - ENOMEM: Memory allocation failure.
 *               - Other codes: Errors from the `predicate` function.
 *
 * @returns    - Pointer to a dynamically allocated array of matched elements (free with `free()`).
 *             - NULL if no matches or an error occurred. Check `*found_size` to distinguish cases:
 *                 - `0`: No matches.
 *                 - `(size_t)-1`: Error (see `errno`).
 *
 * @note       - Elements are copied via `memcpy()`, so they must be trivially copyable.
 *             - Predicate arguments are reinitialized for each element via `va_copy`.
 *             - Not thread-safe if the predicate or its arguments rely on shared state.
 */
void * predicate_search(const void *array, size_t size, size_t type_size, size_t *found_size,
                        predicate_func_t predicate, ...) {
    // Checking if the arguments (array, type_size, found_size and predicate) are valid.
    if (array == NULL || type_size == 0 || found_size == NULL || predicate == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning NULL).
        errno = EINVAL;
        
        if (found_size != NULL) {
            // If found_size is not NULL, assigning to found_size the special "undefined" value `(size_t)-1`.
            *found_size = (size_t)-1;
        }
        
        return NULL;
    }
    
    // Checking if the total array size does not exceed the system limits (SIZE_MAX).
    if (size > SIZE_MAX / type_size) {
        /*
         * If exceeds, interrupting the function
         * (setting errno to EOVERFLOW, assigning to found_size the special "undefined" value `(size_t)-1`
         * and returning NULL).
         */
        errno = EOVERFLOW;
        *found_size = (size_t)-1;
        return NULL;
    }
    
    // Initializing array where the found elements will be written and its size.
    size_t found_capacity = 16;
    *found_size = 0;
    void *found = malloc(found_capacity * type_size);
    
    // Checking the success of memory allocation.
    if (found == NULL) {
        // If failed, terminating the function with error report.
        errno = ENOMEM;
        *found_size = (size_t)-1;
        return NULL;
    }
    
    // Getting the predicate arguments.
    va_list predicate_args;
    va_start(predicate_args, predicate);
    
    for (size_t i = 0; i < size; i++) {
        // Getting the address of the i-th element.
        const void *current_element = (const void *)((const uintptr_t)array + i * type_size);
        
        // Getting the predicate match result.
        int saved_errno = errno;
        
        va_list predicate_args_copy;
        va_copy(predicate_args_copy, predicate_args);
        
        bool match = predicate(current_element, predicate_args_copy);
        
        va_end(predicate_args_copy);
        
        if (errno != saved_errno) {
            /*
             * If the predicate match check was failed, interrupting the function
             * (errno is set, clearing the buffer of found elements (if necessary),
             * assigning to found_size the special "undefined" value `(size_t)-1` and returning NULL).
             */
            va_end(predicate_args);
            free(found);
            *found_size = (size_t)-1;
            return NULL;
        }
        
        if (match) {
            // Resizing the results array to accommodate the new match if necessary.
            if (*found_size >= found_capacity) {
                found_capacity *= 2;
                void *new_found = realloc(found, found_capacity * type_size);
                if (new_found == NULL) {
                    /*
                     * If the memory reallocation was failed, interrupting the function
                     * (setting errno to ENOMEM, clearing the buffer of found elements,
                     * assigning to found_size the special "undefined" value `(size_t)-1` and returning NULL).
                     */
                    va_end(predicate_args);
                    free(found);
                    errno = ENOMEM;
                    *found_size = (size_t)-1;
                    return NULL;
                }
                found = new_found;
            }
            
            // Getting the current element position in the array of found elements.
            void *current_found_element = (void *)((uintptr_t)found + *found_size * type_size);
            
            // Copying current element to the array of found elements.
            memcpy(current_found_element, current_element, type_size);
            
            // Increasing the found elements counter.
            ++*found_size;
        }
    }
    
    // Deleting the predicate arguments.
    va_end(predicate_args);
    
    // Truncating the found elements array to its real size.
    if (*found_size == 0) {
        free(found);
        found = NULL;
    } else {
        void *trimmed = realloc(found, *found_size * type_size);
        if (trimmed != NULL) {
            found = trimmed;
        }
    }
    
    return found;
}

/**
 * @brief      Swaps the contents of two memory blocks of a specified type.
 *
 * @details    This function exchanges the contents of two memory regions pointed to by `a` and `b`,
 *             using a temporary buffer allocated dynamically. It handles NULL pointers, zero size,
 *             and memory allocation failures.
 *
 * @param[in,out] a          Pointer to the first memory block (must not be NULL).
 * @param[in,out] b          Pointer to the second memory block (must not be NULL).
 * @param[in]     type_size  Size of the data type in bytes (must be >0).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: `a`, `b` are NULL or `type_size == 0`.
 *               - ENOMEM: Memory allocation failure for the temporary buffer.
 *
 * @returns    - `0`:  Success.
 *             - `-1`: Error (invalid arguments or memory allocation failure).
 *
 * @note       - Works with trivially copyable types (e.g., `int`, `float`, structs without pointers).
 *             - Not thread-safe due to reliance on `errno`.
 *             - Uses `memmove()` for safe overlapping memory handling.
 */
static int swap(void *a, void *b, size_t type_size) {
    // Checking if the arguments (a, b, type_size) are valid.
    if (a == NULL || b == NULL || type_size == 0) {
        // If not, interrupting the function (setting errno to EINVAL and returning -1).
        errno = EINVAL;
        return -1;
    }
    
    // If `a` is equal to `b`, there is nothing to swap. Returning 0.
    if (a == b) {
        return 0;
    }
    
    // Trying to allocate the memory for temporary variable.
    void *tmp = malloc(type_size);
    if (tmp == NULL) {
        // If failed, interrupting the function (setting errno to ENOMEM and returning -1).
        errno = ENOMEM;
        return -1;
    }
    
    // Swapping the values of variables.
    memmove(tmp, a, type_size);
    memmove(a, b, type_size);
    memmove(b, tmp, type_size);
    
    // Clearing the memory where the temporary variable is.
    free(tmp);
    
    // Everything is ok, returning 0.
    return 0;
}

/**
 * @brief      Sorts an array using the quicksort algorithm.
 *
 * @details    This function implements a recursive quicksort algorithm. It selects a pivot element,
 *             partitions the array into sub-arrays, and sorts them. Uses `swap()` for element exchange.
 *
 * @param[in,out] array     Pointer to the array to sort (must not be NULL).
 * @param[in]     size      Number of elements in the array.
 * @param[in]     type_size Size of each element in bytes (must be >0).
 * @param[in]     compare   Comparison function (must not be NULL).
 *                          Signature: `int compare(const void *, const void *)`.
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: `array`, `compare` are NULL or `type_size == 0`.
 *               - EOVERFLOW: `size * type_size` exceeds `SIZE_MAX`.
 *               - ENOMEM: Memory allocation failure in `swap()`.
 *               - Other errors: Errors from `compare`.
 *
 * @returns    - `0`:  Success.
 *             - `-1`: Error (invalid arguments, overflow, or memory allocation failure).
 *
 * @note       - Works with trivially copyable types (e.g., `int`, `float`).
 *             - Not thread-safe due to reliance on `errno`.
 */
int quicksort(void *array, size_t size, size_t type_size, compare_func_t compare) {
    // Checking if the arguments (array, type_size and compare) are valid.
    if (array == NULL || type_size == 0 || compare == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning -1).
        errno = EINVAL;
        return -1;
    }
    
    // Checking if the total array size does not exceed the system limits (SIZE_MAX).
    if (size > SIZE_MAX / type_size) {
        /*
         * If exceeds, interrupting the function
         * (setting errno to EOVERFLOW, and returning -1).
         */
        errno = EOVERFLOW;
        return -1;
    }
    
    if (size <= 1) {
        // If the size is 1 or less, the array is already sorted.
        return 0;
    }
    
    // Creating a stack to store the boundaries of the current segments.
    size_t range_stack_capacity = 64;
    size_t *range_stack = malloc(range_stack_capacity * 2 * sizeof(size_t));
    if (range_stack == NULL) {
        // If the stack allocation wasn't successful, interrupting the function (setting errno to ENOMEM, returning -1).
        errno = ENOMEM;
        return -1;
    }
    
    // Starting with the whole array.
    size_t range_stack_top = 0;
    range_stack[range_stack_top++] = 0;
    range_stack[range_stack_top++] = size - 1;
    
    // Sorting.
    while (range_stack_top > 0) {
        // Getting the boundaries of the current segment.
        size_t high = range_stack[--range_stack_top];
        size_t low = range_stack[--range_stack_top];
        
        if (low < high) {
            // Middle element index.
            size_t mid = low + (high - low) / 2;
            
            // Left, right and middle elements' pointers.
            void *low_ptr = (void *)((uintptr_t)array + low * type_size);
            void *mid_ptr = (void *)((uintptr_t)array + mid * type_size);
            void *high_ptr = (void *)((uintptr_t)array + high * type_size);
            
            // Comparing all the elements between each other.
            int saved_errno = errno;
            int cmp_low_mid = compare((const void *)low_ptr, (const void *)mid_ptr);
            if (errno != saved_errno) {
                // If the comparison went wrong, terminating the function (deallocating the stack, returning -1).
                free(range_stack);
                return -1;
            }
            
            saved_errno = errno;
            int cmp_low_high = compare((const void *)low_ptr, (const void *)high_ptr);
            if (errno != saved_errno) {
                // Similarly.
                free(range_stack);
                return -1;
            }
            
            saved_errno = errno;
            int cmp_mid_high = compare((const void *)mid_ptr, (const void *)high_ptr);
            if (errno != saved_errno) {
                // Similarly.
                free(range_stack);
                return -1;
            }
            
            // Getting a median of low, middle and high elements.
            void *median = high_ptr;
            if (cmp_low_mid < 0) {
                // low < mid.
                if (cmp_mid_high < 0) {
                    // mid < high, mid is a median.
                    median = mid_ptr;
                } else if (cmp_low_high >= 0) {
                    // low >= high, low is a median.
                    median = low_ptr;
                }
            } else {
                // low >= mid;
                if (cmp_mid_high > 0) {
                    // mid > high, mid is a median.
                    median = mid_ptr;
                } else if (cmp_low_high <= 0) {
                    // low <= high, low is a median.
                    median = low_ptr;
                }
            }
            
            // Putting the median to the high position.
            if (swap(median, high_ptr, type_size) == -1) {
                free(range_stack);
                return -1;
            }
            
            // Getting the address of pivot element. It will be the rightmost element.
            void *pivot = high_ptr;
            
            // Getting the addresses of left and right elements. The right element is the element before pivot.
            void *left = low_ptr;
            void *right = (void *)((uintptr_t)pivot - type_size);
            
            while (left <= right) {
                // Looking for the leftmost element greater than or equal to pivot.
                saved_errno = errno;
                while (compare((const void *)left, (const void *)pivot) < 0) {
                    if (errno != saved_errno) {
                        free(range_stack);
                        return -1;
                    }
                    
                    left = (void *)((uintptr_t)left + type_size);
                    
                    if (left > right) {
                        break;
                    }
                }
                
                // Looking for the rightmost element less than or equal to pivot.
                saved_errno = errno;
                while (right >= low_ptr && compare((const void *)right, (const void *)pivot) > 0) {
                    if (errno != saved_errno) {
                        free(range_stack);
                        return -1;
                    }
                    
                    right = (void *)((uintptr_t)right - type_size);
                    
                    if (right < low_ptr) {
                        break;
                    }
                }
                
                // If left and right do not intersect, swap them.
                if (left <= right) {
                    if (swap(left, right, type_size) == -1) {
                        // If `swap()` failed, returning -1.
                        free(range_stack);
                        return -1;
                    }
                    
                    left = (void *)((uintptr_t)left + type_size);
                    right = (void *)((uintptr_t)right - type_size);
                }
            }
            
            // Trying to put pivot in the right place.
            if (swap(left, pivot, type_size) == -1) {
                // If failure, interrupting the function (errno is already set, freeing the stack returning -1).
                free(range_stack);
                return -1;
            }
            
            size_t pivot_pos = ((uintptr_t)left - (uintptr_t)array) / type_size;
            
            // Reallocating the stack if there is no memory remaining.
            if (range_stack_top + 4 > range_stack_capacity * 2) {
                size_t new_range_stack_capacity = range_stack_capacity * 2;
                size_t *new_range_stack = (size_t *)realloc(
                    range_stack,
                    new_range_stack_capacity * 2 * sizeof(size_t)
                );
                if (new_range_stack == NULL) {
                    free(range_stack);
                    errno = ENOMEM;
                    return -1;
                }
                range_stack = new_range_stack;
                range_stack_capacity = new_range_stack_capacity;
            }
            
            // Adding next ranges to stack (if need).
            if (pivot_pos != 0 && pivot_pos != size - 1) {
                if (pivot_pos - low > high - pivot_pos) {
                    range_stack[range_stack_top++] = low;
                    range_stack[range_stack_top++] = pivot_pos - 1;
                    
                    range_stack[range_stack_top++] = pivot_pos + 1;
                    range_stack[range_stack_top++] = high;
                } else {
                    range_stack[range_stack_top++] = pivot_pos + 1;
                    range_stack[range_stack_top++] = high;
                    
                    range_stack[range_stack_top++] = low;
                    range_stack[range_stack_top++] = pivot_pos - 1;
                }
            }
        }
    }
    
    free(range_stack);
    return 0;
}

/**
 * @brief      Sorts an array using selection sort algorithm.
 *
 * @details    Sorts elements of `array` in ascending order using selection sort.
 *             The function assumes that `compare` is compatible with the data layout (alignment, type_size).
 *             Elements are compared via `compare`, and swapped via `swap`.
 *
 * @param[in]  array       Pointer to the start of the array (must not be NULL).
 * @param[in]  size        Number of elements in the array.
 * @param[in]  type_size   Size of one element in bytes (must be >0).
 * @param[in]  compare     Comparison function returning <0, 0, or >0 (must not be NULL).
 *
 * @errors     Sets `errno` to:
 *               - EINVAL: Invalid arguments (NULL `array`/`compare`, `type_size == 0`).
 *               - EOVERFLOW: `size * type_size` exceeds `SIZE_MAX`.
 *               - Other errors from `compare` or `swap`.
 *
 * @returns    - 0: Success.
 *             - -1: Error (check `errno`).
 *
 * @note       - Performance: O(n²) time complexity.
 *             - Not thread-safe due to `errno` usage
 */
int selection_sort(void *array, size_t size, size_t type_size, compare_func_t compare) {
    // Checking if the arguments (array, type_size and compare) are valid.
    if (array == NULL || type_size == 0 || compare == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning NULL).
        errno = EINVAL;
        return -1;
    }
    
    // Checking if the total array size does not exceed the system limits (SIZE_MAX).
    if (size > SIZE_MAX / type_size) {
        /*
         * If exceeds, interrupting the function
         * (setting errno to EOVERFLOW, and returning -1).
         */
        errno = EOVERFLOW;
        return -1;
    }
    
    // If the size is 0, the array is already sorted.
    if (size > 0) {
        for (size_t i = 0; i < size - 1; i++) {
            // Getting the address of the i-th element.
            void *ith_element = (void *)((uintptr_t)array + i * type_size);
            
            // Finding the minimum element on the right to the current.
            void *min_element = (void *)((uintptr_t)array + i * type_size);
            for (size_t j = i + 1; j < size; j++) {
                // Getting the address of the j-th element.
                void *jth_element = (void *)((uintptr_t)array + j * type_size);
                
                // Getting the comparison result.
                int saved_errno = errno;
                int compare_result = compare((const void *)jth_element, (const void *)min_element);
                if (errno != saved_errno) {
                    // If comparison failure, terminating the function (errno is set, returning -1).
                    return -1;
                }
                
                if (compare_result < 0) {
                    // If j-th element is less than the current minimum element, then update the minimum element.
                    min_element = (void *)((uintptr_t)array + j * type_size);
                }
            }
            
            // Trying to put the min element in the right place
            if (swap(ith_element, min_element, type_size) == -1) {
                // If failure, interrupting the function (errno is already set, returning -1).
                return -1;
            }
        }
    }
    
    // Everything is ok, returning 0.
    return 0;
}

/**
 * @brief      Sorts an array using insertion sort algorithm.
 *
 * @details    Sorts elements of `array` in ascending order using insertion sort.
 *             The function assumes that `compare` is compatible with the data layout (alignment, type_size).
 *             Elements are compared via `compare`, and swapped via `swap`.
 *
 * @param[in]  array       Pointer to the start of the array (must not be NULL).
 * @param[in]  size        Number of elements in the array.
 * @param[in]  type_size   Size of one element in bytes (must be >0).
 * @param[in]  compare     Comparison function returning <0, 0, or >0 (must not be NULL).
 *
 * @errors     Sets `errno` to:
 *               - EINVAL: Invalid arguments (NULL `array`/`compare`, `type_size == 0`).
 *               - EOVERFLOW: `size * type_size` exceeds `SIZE_MAX`.
 *               - Other errors from `compare` or `swap`.
 *
 * @returns    - 0: Success.
 *             - -1: Error (check `errno`).
 *
 * @note       - Performance: O(n²) time complexity.
 *             - Not thread-safe due to `errno` usage
 */
int insertion_sort(void *array, size_t size, size_t type_size, compare_func_t compare) {
    // Checking if the arguments (array, type_size and compare) are valid.
    if (array == NULL || type_size == 0 || compare == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning NULL).
        errno = EINVAL;
        return -1;
    }
    
    // Checking if the total array size does not exceed the system limits (SIZE_MAX).
    if (size > SIZE_MAX / type_size) {
        /*
         * If exceeds, interrupting the function
         * (setting errno to EOVERFLOW, and returning -1).
         */
        errno = EOVERFLOW;
        return -1;
    }
    
    for (size_t i = 1; i < size; i++) {
        for (size_t j = i; j > 0; j--) {
            // Getting the addresses of j-th and (j-1)-th element.
            void *jth_element = (void *)((uintptr_t)array + j * type_size);
            void *element_before_jth = (void *)((uintptr_t)array + (j - 1) * type_size);
            
            // Getting the comparison result.
            int saved_errno = errno;
            int compare_result = compare((const void *)element_before_jth, (const void *)jth_element);
            if (saved_errno != errno) {
                // If comparison failure, terminating the function (errno is set, returning -1).
                return -1;
            }
            
            if (compare_result < 0) {
                // If (j-1)-th element is less than j-th, then we got to the sorted part of array.
                break;
            }
            
            // Trying to swap j-th and (j-1)-th elements.
            if (swap(jth_element, element_before_jth, type_size) == -1) {
                // If failure, interrupting the function (errno is already set, returning -1).
                return -1;
            }
        }
    }
    
    // Everything is ok, returning 0.
    return 0;
}