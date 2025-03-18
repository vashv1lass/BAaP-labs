#include "algorithms.h"

#include <stddef.h> // size_t, ptrdiff_t
#include <string.h> // memcpy
#include <errno.h>  // errno, errno codes
#include <stdlib.h> // malloc, calloc, realloc, free
#include <stdarg.h> // va_list, va_start, va_end, va_copy

#include "auxiliary.h"

/**
 * @brief      Searches for the specified element in an array using a linear search.
 *
 * @note       The function can work with arrays of any type, the main thing is that for these types a comparison
 *             function needs to be defined. Also you need to specify the size of array element type in bytes.
 *
 * @note       The comparison function must take two arguments of type "const void *" and return an integer:
 * 		       negative if the first argument is less than the second one,
 * 		       positive if the first argument is greater than the second one,
 * 		       and zero if the arguments are equal.
 *
 * @param[in]  array      The array in which to find the element
 * @param[in]  size       Size of the \p array
 * @param[in]  target     The element to be found in \p array
 * @param[in]  type_size  The size of \p array element type (in bytes)
 * @param[in]  compare    Comparison function
 * @param[out] found_size The size of array of found elements
 *
 * @returns    The array of found elements.
 */
void * linear_search(const void *array, size_t size, const void *target, size_t type_size, compare_func_t compare,
                     size_t *found_size) {
	// verifying the validity of the passed arguments
	VERIFY(array != NULL && target != NULL && type_size > 0 && compare != NULL && found_size != NULL, NULL, EINVAL);

	// initializing array where the found elements will be written and its size
	void *found = NULL;
	*found_size = 0;

	for (size_t i = 0; i < size; i++) {
		/*
	     * HOW TO GET AN ELEMENT IN "(const) void *" ARRAY:
	     *
	     * 1) convert array pointer to "(const) char *" type for byte-wise access
	     * 2) get the distance from the beginning of the array to the pointer to the first byte of the needed element
	     * 3) convert the received value to the pointer difference type (ptrdiff_t)
	     * 4) get a pointer to the first byte of the desired element by adding the distance to the pointer to the
	     *    beginning of the array
	     * 5) convert the received pointer back to "(const) void *" (this will be the pointer that is needed)
	     */
		if (compare((const void *)((const char *)array + (ptrdiff_t)(i * type_size)), target) == 0) {
			// if current element is equal to target, then add it to the array of found elements
			/*
			 * reallocating the memory for the array of found elements and verifying the success of reallocation,
			 * if unsuccessful, then clearing the memory
			 */
			void *new_found = realloc(found, ++(*found_size) * type_size);
			if (new_found == NULL) {
				if (found != NULL) {
					free(found);
				}
				errno = ENOMEM;
			}
			found = new_found;
			VERIFY(found != NULL, NULL, ENOMEM);

			// copying current element to the array of found elements
			memcpy((void *)((char *)found + (ptrdiff_t)((*found_size - 1) * type_size)),
				   (const void *)((const char *)array + (ptrdiff_t)(i * type_size)),
				   type_size);
		}
	}

	return found;
}

/**
 * @brief      Searches for the specified element in sorted array using a binary search.
 *
 * @note       The function can work with arrays of any type, the main thing is that for these types a comparison
 *             function needs to be defined. Also you need to specify the size of array element type in bytes.
 *
 * @note       The comparison function must take two arguments of type "const void *" and return an integer:
 * 		       negative if the first argument is less than the second one,
 * 		       positive if the first argument is greater than the second one,
 * 		       and zero if the arguments are equal.
 *
 * @param[in]  array      The array in which to find the element
 * @param[in]  size       Size of the \p array
 * @param[in]  target     The element to be found in \p array
 * @param[in]  type_size  The size of \p array element type (in bytes)
 * @param[in]  compare    Comparison function
 * @param[out] found_size The size of array of found elements
 *
 * @returns    The array of found elements.
 */
void * binary_search(const void *array, size_t size, const void *target, size_t type_size, compare_func_t compare,
					 size_t *found_size) {
	// verifying the validity of the passed arguments
	VERIFY(type_size > 0 && array != NULL && target != NULL && compare != NULL && found_size != NULL, NULL, EINVAL);

	// initializing array where the found elements will be written and its size
	void *found = NULL;
	*found_size = 0;

	// initializing the binary search bounds and the index of found element
	ptrdiff_t left = 0;
	ptrdiff_t right = (ptrdiff_t)size - 1;
	ptrdiff_t found_idx = -1;

	// finding the index of target element
	while (left <= right) {
		// see linear_search function to figure out how to get an element in "const void *" array
		ptrdiff_t middle = left + (right - left) / 2;
		int compare_result = compare((const void *)((const char *)array + (ptrdiff_t)(middle * type_size)), target);

		if (compare_result < 0) {
			// if middle element is less than target, then left bound needs to be set to middle
			left = middle + 1;
		} else if (compare_result > 0) {
			// if greater, then right bound needs to be set to middle
			right = middle - 1;
		} else {
			// if equal, we found it!
			found_idx = middle;
			break;
		}
	}

	if (found_idx != -1) {
		// if the target element was found:
		// finding the leftmost element that is equal to target
		ptrdiff_t first = found_idx;
		while (first > 0 &&
		       compare((const void *)((const char *)array + (ptrdiff_t)((first - 1) * type_size)), target) == 0) {
			--first;
		}

		// finding the rightmost element that is equal to target
		ptrdiff_t last = found_idx;
		while (last < (ptrdiff_t)size - 1 &&
		       compare((const void *)((const char *)array + (ptrdiff_t)((last + 1) * type_size)), target) == 0) {
			++last;
		}

		// allocating memory for the found elements and verifying the success of the allocation
		found = calloc((size_t)(last - first + 1), type_size);
		VERIFY(found != NULL, NULL, ENOMEM);

		// the size of the array also needs to be set
		*found_size = (size_t)(last - first + 1);

		// copying all the found elements to an array
		for (size_t i = 0; i < *found_size; i++) {
			memcpy((void *)((char *)found + (ptrdiff_t)(i * type_size)),
			       (const void *)((const char *)array + (ptrdiff_t)((first + i) * type_size)),
				   type_size);
		}
	}

	return found;
}

/**
 * @brief          Swaps the values of two variables.
 *
 * @note           The function can work with variables of any type, you just need to specify the size of variables type
 *                 in bytes.
 *
 * @param[in, out] a         First variable
 * @param[in, out] b         Second variable
 * @param[in]      type_size The size of variables type (in bytes)
 *
 * @returns        None.
 */
static void swap(void *a, void *b, size_t type_size) {
	// verifying the validity of the passed arguments
	VERIFY(type_size > 0 && a != NULL && b != NULL, NOTHING, EINVAL);

	// allocating memory for temporary variable and verifying the success of memory allocation
	void *tmp = malloc(type_size);
	VERIFY(tmp != NULL, NOTHING, ENOMEM);

	// swapping the values of variables
	memcpy(tmp, a, type_size);
	memcpy(a, b, type_size);
	memcpy(b, tmp, type_size);

	// clearing the memory where the temporary variable is
	free(tmp);
}

/**
 * @brief     Sorts the array according to the passed comparison function (using the quick sort algorithm).
 *
 * @note      The function can work with arrays of any type, the main thing is that for these types a comparison
 *            function needs to be defined. Also you need to specify the size of array element type in bytes.
 *
 * @note      The comparison function must take two arguments of type "const void *" and return an integer:
 * 		      negative if the first argument is less than the second one,
 * 		      positive if the first argument is greater than the second one,
 * 		      and zero if the arguments are equal.
 *
 * @param[in] array     Array to be sorted
 * @param[in] size      Size of the /p array
 * @param[in] type_size The size of /p array element type (in bytes)
 * @param[in] compare   Comparison function
 *
 * @returns   None.
 */
void quicksort(void *array, size_t size, size_t type_size, compare_func_t compare) {
	// verifying the validity of the passed arguments
	VERIFY(type_size > 0 && array != NULL && compare != NULL, NOTHING, EINVAL);

	if (size > 1) {
		// see linear_search function to figure out how to get an element in "void *" array
		void *pivot = (void *)((char *)array + (ptrdiff_t)((size - 1) * type_size));
		void *left = array;
		void *right = (void *)((char *)pivot - (ptrdiff_t)type_size);

		while (left <= right) {
			// looking for the leftmost element greater than pivot
			while (compare((const void *)left, (const void *)pivot) < 0) {
				left += type_size;
			}

			// looking for the rightmost element less than pivot
			while (right >= array && compare((const void *)right, (const void *)pivot) > 0) {
				right -= type_size;
			}

			// if left and right do not intersect, swap them
			if (left <= right) {
				swap(left, right, type_size);
				left += type_size;
				right -= type_size;
			}
		}

		// putting pivot in the right place
		swap(left, pivot, type_size);

		// getting left and right part sizes
		size_t left_size = (size_t)((char *)left - (char *)array) / type_size;
		size_t right_size = size - left_size - 1;

		// starting recursively from left and right
		quicksort(array, left_size, type_size, compare);
		quicksort((void *)((char *)left + (ptrdiff_t)type_size), right_size, type_size, compare);
	}
}

/**
 * @brief     Sorts the array according to the passed comparison function (using the selection sort algorithm).
 *
 * @note      The function can work with arrays of any type, the main thing is that for these types a comparison
 *            function needs to be defined. Also you need to specify the size of array element type in bytes.
 *
 * @note      The comparison function must take two arguments of type "const void *" and return an integer:
 * 		      negative if the first argument is less than the second one,
 * 		      positive if the first argument is greater than the second one,
 * 		      and zero if the arguments are equal.
 *
 * @param[in] array     Array to be sorted
 * @param[in] size      Size of the /p array
 * @param[in] type_size The size of /p array element type (in bytes)
 * @param[in] compare   Comparison function
 *
 * @returns   None.
 */
void selection_sort(void *array, size_t size, size_t type_size, compare_func_t compare) {
	// verifying the validity of the passed arguments
	VERIFY(type_size > 0 && array != NULL && compare != NULL, NOTHING, EINVAL);

	if (size > 0) {
		for (size_t i = 0; i < size - 1; i++) {
			size_t min_index = i;
			for (size_t j = i + 1; j < size; j++) {
				// finding the min element index on the right to the current
				// see linear_search function to figure out how to get an element in "void *" array
				if (compare((const void *)((const char *)array + (ptrdiff_t)(j * type_size)),
							(const void *)((const char *)array + (ptrdiff_t)(min_index * type_size))) < 0) {
					min_index = j;
				}
			}

			// putting the min element in the right place
			swap((void *)((char *)array + (ptrdiff_t)(i * type_size)),
				 (void *)((char *)array + (ptrdiff_t)(min_index * type_size)),
				 type_size);
		}
	}
}

/**
 * @brief     Sorts the array according to the passed comparison function (using the insertion sort algorithm).
 *
 * @note      The function can work with arrays of any type, the main thing is that for these types a comparison
 *            function needs to be defined. Also you need to specify the size of array element type in bytes.
 *
 * @note      The comparison function must take two arguments of type "const void *" and return an integer:
 * 		      negative if the first argument is less than the second one,
 * 		      positive if the first argument is greater than the second one,
 * 		      and zero if the arguments are equal.
 *
 * @param[in] array     Array to be sorted
 * @param[in] size      Size of the /p array
 * @param[in] type_size The size of /p array element type (in bytes)
 * @param[in] compare   Comparison function
 *
 * @returns   None.
 */
void insertion_sort(void *array, size_t size, size_t type_size, compare_func_t compare) {
	// verifying the validity of the passed arguments
	VERIFY(type_size > 0 && array != NULL && compare != NULL, NOTHING, EINVAL);

	for (size_t i = 1; i < size; i++) {
		// putting the element in the right place in the sorted part of the array
		// see linear_search function to figure out how to get an element in "void *" array
		for (size_t j = i;
			 j > 0 &&
			 compare((const void *)((const char *)array + (ptrdiff_t)(j * type_size)),
					 (const void *)((const char *)array + (ptrdiff_t)((j - 1) * type_size))) < 0;
			 j--) {
			swap((void *)((char *)array + (ptrdiff_t)(j * type_size)),
				 (void *)((char *)array + (ptrdiff_t)((j - 1) * type_size)),
				 type_size);
		}
	}
}

/**
 * @brief      Searches for all the elements that satisfy the condition.
 *
 * @note       The function can work with arrays of any type, the main thing is that you need to specify the size of
 *             array element type in bytes. Also you need to pass the predicate
 *
 * @note       The predicate function must take one argument of type "const void *" and a variadic list of arguments
 * 			   that checks whether the condition is satisfied. It must return a boolean value:
 * 		       true if the first argument satisfies the condition,
 * 		       false if not.
 *
 * @param[in]  array      The array in which to find the element
 * @param[in]  size       Size of the \p array
 * @param[in]  type_size  The size of \p array element type (in bytes)
 * @param[out] found_size The size of array of found elements
 * @param[in]  predicate  Predicate function
 * @param[in]  ...        Variadic list of predicate function arguments
 *
 * @returns    The array of found elements.
 */
void * predicate_search(const void *array, size_t size, size_t type_size, size_t *found_size,
						predicate_func_t predicate, ...) {
	// verifying the validity of the passed arguments
	VERIFY(type_size > 0 && array != NULL && predicate != NULL && found_size != NULL, NULL, EINVAL);

	// initializing array where the found elements will be written and its size
	*found_size = 0;
	void *found = NULL;

	// getting predicate arguments
	va_list args;
	va_start(args, predicate);

	for (size_t i = 0; i < size; i++) {
		// getting predicate arguments copy (need for safety)
		va_list args_copy;
		va_copy(args_copy, args);

		// see linear_search function to figure out how to get an element in "const void *" array
		if (predicate((const void *)(const char *)array + (ptrdiff_t)(i * type_size), args_copy)) {
			/*
			 * if current satisfies the condition, then add it to the array of found elements like it is in the
			 * linear_search function
			 */
			void *new_found = realloc(found, ++(*found_size) * type_size);
			if (new_found == NULL) {
				free(found);
				errno = ENOMEM;
			}
			found = new_found;
			VERIFY(found != NULL, NULL, ENOMEM);

			memcpy((void *)((char *)found + (ptrdiff_t)((*found_size - 1) * type_size)),
			       (const void *)((const char *)array + (ptrdiff_t)(i * type_size)),
				   type_size);
		}

		va_end(args_copy);
	}

	va_end(args);

	return found;
}