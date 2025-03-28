#include "algorithms.h"

#include <stddef.h> // For size_t.
#include <errno.h>  // For `errno` and its codes.
#include <stdlib.h> // For `calloc()`, `free()`, `SIZE_MAX` and other.
#include <memory.h> // For `memcpy()`.
#include <stdarg.h> // For `va_list`, `va_start()` and other.

#include "auxiliary.h" // For compare_func_t, predicate_func_t.

/**
 * @brief      Performs a linear search for elements matching a target in a generic array.
 *
 * @details    Iterates through the array, compares each element with the target using a comparator function,
 *             and collects all matches into a dynamically allocated result array. The caller must free the
 *             returned buffer. If no matches are found, returns NULL with `*found_size = 0`.
 *
 * @param[in]  array           Pointer to the start of the array (must not be NULL).
 * @param[in]  size            Number of elements in the array.
 * @param[in]  target_element  Pointer to the element to search for (must not be NULL).
 * @param[in]  type_size       Size of each element in bytes (> 0).
 * @param[in]  compare         Comparator function returning 0 for matching elements (must not be NULL).
 * @param[out] found_size      Receives the number of matches. Set to `(size_t)-1` on error, 0 if no matches.
 *
 * @errors     Sets `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers, type_size = 0).
 *               - ENOMEM: Memory allocation failure during results collection.
 *               - EOVERFLOW: The total size (`size * type_size`) exceeds `SIZE_MAX`
 *
 * @returns    On success: Pointer to the array of matches (free with `free()`).
 *             No matches: NULL with `*found_size = 0`.
 *             On error:   NULL with `*found_size = (size_t)-1` (check `errno`).
 */
void * linear_search(const void *array, size_t size, const void *target_element, size_t type_size,
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
		 * (setting errno to EOVERFLOW, assigning to found_size the special "undefined" value `(size_t)-1`
		 * and returning NULL).
		 */
		errno = EOVERFLOW;
		*found_size = (size_t)-1;
		return NULL;
	}
	
	// Initializing array where the found elements will be written and its size.
	void *found = NULL;
	*found_size = 0;
	
	for (size_t i = 0; i < size; i++) {
		// Getting the address of the i-th element.
		const void *current_element = (const void *)((const uintptr_t)array + i * type_size);
		
		if (compare(current_element, target_element) == 0) {
			// Resizing the results array to accommodate the new match.
			void *new_found = realloc(found, ++(*found_size) * type_size);
			if (new_found == NULL) {
				/*
				 * If the memory reallocation was failed, interrupting the function
				 * (setting errno to ENOMEM, clearing the buffer of found elements (if necessary),
				 * assigning to found_size the special "undefined" value `(size_t)-1` and returning NULL).
				 */
				errno = ENOMEM;
				
				if (found != NULL) {
					free(found);
				}
				
				*found_size = (size_t)-1;
				
				return NULL;
			}
			found = new_found;
			
			// Getting the current element position in the array of found elements.
			void *current_found_element = (void *)((uintptr_t)found + (*found_size - 1) * type_size);
			
			// Copying current element to the array of found elements.
			memcpy(current_found_element, current_element, type_size);
		}
	}
	
	// Everything is ok, returning the array of found elements.
	return found;
}

/**
 * @brief      Performs a binary search for elements matching a target in a sorted generic array.
 *
 * @details    Searches a sorted array using binary search to locate the first occurrence of the target,
 *             then expands to collect all contiguous matches. The input array must be sorted in
 *             non-decreasing order according to the comparator. The caller must free the returned buffer.
 *             If no matches are found, returns NULL with `*found_size = 0`.
 *
 * @param[in]  array           Pointer to the sorted array (must not be NULL).
 * @param[in]  size            Number of elements in the array.
 * @param[in]  target_element  Pointer to the element to search for (must not be NULL).
 * @param[in]  type_size       Size of each element in bytes (> 0).
 * @param[in]  compare         Comparator function returning 0 for matching elements (must not be NULL).
 * @param[out] found_size      Receives the number of matches. Set to `(size_t)-1` on error, 0 if no matches.
 *
 * @errors     The function may fail and set `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers, type_size = 0).
 *               - EOVERFLOW: The total size (`size * type_size`) exceeds `SIZE_MAX`.
 *               - ENOMEM: Memory allocation failure during results collection.
 *
 * @returns    On success: Pointer to the array of matches (free with `free()`).
 *             No matches: NULL with `*found_size = 0`.
 *             On error:   NULL with `*found_size = (size_t)-1` (check `errno`).
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
	
	// Initializing array where the found elements will be written and its size (for now, it is undefined).
	void *found = NULL;
	*found_size = (size_t)-1;
	
	// Checking if the total array size does not exceed the system limits (SIZE_MAX).
	if (size > SIZE_MAX / type_size) {
		/*
		 * If exceeds, interrupting the function
		 * (setting errno to EOVERFLOW, and returning NULL).
		 */
		errno = EOVERFLOW;
		return NULL;
	}
	
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
		int compare_result = compare(middle_element, target_element);
		
		if (compare_result < 0) {
			// If the middle element is less than target element, then left bound needs to be set to middle.
			left = middle + 1;
		} else if (compare_result > 0) {
			// If greater, then right bound needs to be set to middle.
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
		size_t leftmost_idx = found_idx;
		while (leftmost_idx > 0) {
			const void *current_element = (const void *)((const uintptr_t)array + (leftmost_idx - 1) * type_size);
			
			if (compare(current_element, target_element) != 0) {
				break;
			}
			
			--leftmost_idx;
		}
		
		// Finding the index of rightmost element that is equal to target element.
		size_t rightmost_idx = found_idx;
		while (rightmost_idx < size - 1) {
			const void *current_element = (const void *)((const uintptr_t)array + (rightmost_idx + 1) * type_size);
			
			if (compare(current_element, target_element) != 0) {
				break;
			}
			
			++rightmost_idx;
		}
		
		// Trying to allocate memory for the found elements.
		found = calloc(rightmost_idx - leftmost_idx + 1, type_size);
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
		
		// Copying all the found elements to an array.
		for (size_t i = 0; i < *found_size; i++) {
			// Getting the current element position in the array of found elements and in source array.
			void *current_found_element = (void *)((uintptr_t)found + i * type_size);
			const void *current_found_element_src =
					(const void *)((const uintptr_t)array + (leftmost_idx + i) * type_size);
			
			memcpy(current_found_element, current_found_element_src, type_size);
		}
	} else {
		// If not, setting the size of found elements to 0.
		*found_size = 0;
	}
	
	// Everything is ok, returning the found elements array.
	return found;
}

/**
 * @brief      Performs a predicate-based search for elements in a generic array.
 *
 * @details    Iterates through the array, evaluates each element against a predicate function with variable arguments,
 *             and collects all matches into a dynamically allocated result array.
 *             The caller must free the returned buffer. If no matches are found, returns NULL with `*found_size = 0`.
 *
 * @param[in]  array           Pointer to the start of the array (must not be NULL).
 * @param[in]  size            Number of elements in the array.
 * @param[in]  type_size       Size of each element in bytes (> 0).
 * @param[out] found_size      Receives the number of matches. Set to `(size_t)-1` on error, 0 if no matches.
 * @param[in]  predicate       Predicate function returning true for matching elements (must not be NULL).
 * @param[in]  ...             Variable arguments forwarded to the predicate function.
 *
 * @errors     Sets `errno` to:
 *               - EINVAL: Invalid arguments (NULL pointers, type_size = 0).
 *               - ENOMEM: Memory allocation failure during results collection.
 *               - EOVERFLOW: The total size (`size * type_size`) exceeds `SIZE_MAX`.
 *
 * @returns    On success: Pointer to the array of matches (free with `free()`).
 *             No matches: NULL with `*found_size = 0`.
 *             On error:   NULL with `*found_size = (size_t)-1` (check `errno`).
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
	void *found = NULL;
	*found_size = 0;
	
	// Getting the predicate arguments.
	va_list predicate_args;
	va_start(predicate_args, predicate);
	
	for (size_t i = 0; i < size; i++) {
		// Getting the address of the i-th element.
		const void *current_element = (const void *)((const uintptr_t)array + i * type_size);
		
		// Getting predicate arguments copy (necessary for safety).
		va_list predicate_args_copy;
		va_copy(predicate_args_copy, predicate_args);
		
		if (predicate(current_element, predicate_args_copy)) {
			// Resizing the results array to accommodate the new match.
			void *new_found = realloc(found, ++(*found_size) * type_size);
			if (new_found == NULL) {
				/*
				 * If the memory reallocation was failed, interrupting the function
				 * (setting errno to ENOMEM, clearing the buffer of found elements (if necessary),
				 * assigning to found_size the special "undefined" value `(size_t)-1` and returning NULL).
				 */
				errno = ENOMEM;
				
				if (found != NULL) {
					free(found);
				}
				
				*found_size = (size_t)-1;
				
				return NULL;
			}
			found = new_found;
			
			// Getting the current element position in the array of found elements.
			void *current_found_element = (void *)((uintptr_t)found + (*found_size - 1) * type_size);
			
			// Copying current element to the array of found elements.
			memcpy(current_found_element, current_element, type_size);
		}
		
		// Deleting the predicate arguments copy.
		va_end(predicate_args_copy);
	}
	
	// Deleting the predicate arguments.
	va_end(predicate_args);
	
	return found;
}

/**
 * @brief     Exchanges the values of two variables using their memory addresses.
 *
 * @details   This function swaps the values of two variables by copying their memory contents.
 *            It uses dynamic memory allocation for a temporary buffer and validates input parameters.
 *
 * @param[in, out] a          Pointer to the first variable (must not be NULL).
 * @param[in, out] b          Pointer to the second variable (must not be NULL).
 * @param[in]      type_size  Size of the data type in bytes (> 0).
 *
 * @errors     The function may fail and set `errno` to:
 *             - EINVAL: Invalid arguments (NULL pointers or \p type_size is zero).
 *             - ENOMEM: Memory allocation failure for the temporary buffer.
 *
 * @returns    On success: 0 (values swapped).
 *             On error:  -1 (check `errno` for details).
 */
static int swap(void *a, void *b, size_t type_size) {
	// Checking if the arguments (a, b, type_size) are valid.
	if (a == NULL || b == NULL || type_size == 0) {
		// If not, interrupting the function (setting errno to EINVAL and returning -1).
		errno = EINVAL;
		return -1;
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
 * @brief          Performs quick sort on a generic array.
 *
 * @details        Sorts the array using the "divide and conquer" approach:
 *                 1. Selects a pivot element (the last element of the subarray).
 *                 2. Reorders elements so that elements less than the pivot are on the left
 *                 and greater ones are on the right.
 *                 3. Recursively applies the algorithm to the left and right partitions.
 *                 Validates input parameters and handles memory-related errors (via `swap()`).
 *
 * @param[in, out] array     Pointer to the start of the array (must not be NULL).
 * @param[in]      size      Number of elements in the array (> 0).
 * @param[in]      type_size Size of one element in bytes (> 0).
 * @param[in]      compare   Pointer to the comparison function (must not be NULL). The function must return:
 *                             - Negative value if the first argument < second.
 *                             - Zero if arguments are equal.
 *                             - Positive value if the first argument > second.
 *
 * @errors         The function may fail and set `errno` to:
 *                   - EINVAL: Invalid arguments (NULL pointers, zero `type_size`, or `size`).
 *                   - EOVERFLOW: Overflow when calculating total array size (size * type_size > SIZE_MAX).
 *                   - Other errors: see `swap()` documentation.
 *
 * @returns        On success: 0.
 *                 On error:   -1 (check `errno`).
 */
int quicksort(void *array, size_t size, size_t type_size, compare_func_t compare) {
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
	
	// If the size is more than 1, the array needs to be sorted.
	if (size > 1) {
		// Getting the address of pivot element. It will be the rightmost element.
		void *pivot = (void *)((uintptr_t)array + (size - 1) * type_size);
		
		// Getting the addresses of left and right elements. The right element is the element before pivot.
		void *left = array;
		void *right = (void *)((uintptr_t)pivot - type_size);
		
		while (left <= right) {
			// Looking for the leftmost element greater than pivot.
			while (compare((const void *)left, (const void *)pivot) < 0) {
				left = (void *)((uintptr_t)left + type_size);
			}
			
			// Looking for the rightmost element less than pivot.
			while (right >= array && compare((const void *)right, (const void *)pivot) > 0) {
				right = (void *)((uintptr_t)right - type_size);
			}
			
			// If left and right do not intersect, swap them.
			if (left <= right) {
				swap(left, right, type_size);
				left += type_size;
				right -= type_size;
			}
		}
		
		// Trying to put pivot in the right place.
		if (swap(left, pivot, type_size) == -1) {
			// If failure, interrupting the function (errno is already set, returning -1).
			return -1;
		}
		
		// Getting the sizes of left and right parts.
		size_t left_size = (size_t)((char *)left - (char *)array) / type_size;
		size_t right_size = size - left_size - 1;
		
		// Returning left and right to their correct places.
		left = array;
		right = (void *)((uintptr_t)array + left_size + type_size);
		
		// Trying to start recursively from left.
		if (quicksort(left, left_size, type_size, compare) == -1) {
			// If failure, interrupting the function (errno is already set, returning -1).
			return -1;
		}
		
		// Trying to start recursively from right.
		if (quicksort(right, right_size, type_size, compare) == -1) {
			// If failure, interrupting the function (errno is already set, returning -1).
			return -1;
		}
	}
	
	// Everything is correct, returning 0.
	return 0;
}

/**
 * @brief          Performs selection sort on a generic array.
 *
 * @details        Sorts the array by repeatedly finding the minimum element in the unsorted portion
 *                 and swapping it with the first unsorted element. The algorithm has O(n²) time complexity.
 *                 Validates input parameters and handles memory-related errors (via `swap()`).
 *
 * @param[in, out] array     Pointer to the start of the array (must not be NULL).
 * @param[in]      size      Number of elements in the array (> 0).
 * @param[in]      type_size Size of one element in bytes (> 0).
 * @param[in]      compare   Pointer to the comparison function (must not be NULL). The function must return:
 *                             - Negative value if the first argument < second.
 *                             - Zero if arguments are equal.
 *                             - Positive value if the first argument > second.
 *
 * @errors         The function may fail and set `errno` to:
 *                   - EINVAL: Invalid arguments (NULL pointers, zero `type_size`, or `size`).
 *                   - EOVERFLOW: Overflow when calculating total array size (size * type_size > SIZE_MAX).
 *                   - Other errors: see `swap()` documentation.
 *
 * @returns        On success: 0.
 *                 On error:  -1 (check `errno`).
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
				
				if (compare((const void *)jth_element, (const void *)min_element) < 0) {
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
 * @brief          Performs insertion sort on a generic array.
 *
 * @details        Sorts the array by iteratively inserting each element into its correct position
 *                 within the sorted subarray. The algorithm has O(n²) time complexity and is efficient
 *                 for small datasets or partially sorted arrays].
 *                 Validates input parameters and handles memory-related errors (via `swap()`).
 *
 * @param[in, out] array     Pointer to the start of the array (must not be NULL).
 * @param[in]      size      Number of elements in the array (> 0).
 * @param[in]      type_size Size of one element in bytes (> 0).
 * @param[in]      compare   Pointer to the comparison function (must not be NULL). The function must return:
 *                             - Negative value if the first argument < second.
 *                             - Zero if arguments are equal.
 *                             - Positive value if the first argument > second.
 *
 * @errors         The function may fail and set `errno` to:
 *                   - EINVAL: Invalid arguments (NULL pointers, zero `type_size`, or `size`).
 *                   - EOVERFLOW: Overflow when calculating total array size (size * type_size > SIZE_MAX).
 *                   - Other errors: see `swap()` documentation.
 *
 * @returns        On success: 0.
 *                 On error:  -1 (check `errno`).
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
			
			if (compare((const void *)element_before_jth, (const void *)jth_element) < 0) {
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