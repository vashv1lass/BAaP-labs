/**
 * @file  database.c
 * @brief Contains the functions (definitions) that may be needed when working with apartments in the context of binary
 *        file.
 */

 #include "database.h"

 #include <stddef.h>  // For `size_t`.
 #include <errno.h>   // For `errno` and its codes.
 #include <stdlib.h>  // For `free()`.
 #include <stdbool.h> // For `bool`, `false`.
 #include <limits.h>  // For `INT_MAX`.
 
 #include "apartment.h"   // For `apartment` and its functions.
 #include "binary_file.h" // For `read_binary_file()`.
 #include "algorithms.h"  // For `predicate_search()`.
 
 /**
  * @brief      Reads an array of apartment structures from a binary file.
  *
  * @details    This function reads a binary file containing contiguous `apartment` structures,
  *             validates data integrity (size must be a multiple of `sizeof(apartment)`),
  *             and returns the parsed array. Uses `read_binary_file()` for low-level I/O.
  *
  * @param[in]  filename   Name of the binary file (must not be NULL).
  * @param[out] array_size Pointer to store the number of `apartment` elements (must not be NULL).
  *                        Set to `(size_t)-1` on error or `0` for empty files.
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: \p filename or \p array_size is NULL.
  *               - ENOMEM: Memory allocation failed (via `read_binary_file()`).
  *               - EILSEQ: File size not a multiple of `sizeof(apartment)`.
  *               - Errors from `read_binary_file()` (e.g., ENOENT, EFBIG).
  *
  * @returns    Pointer to the allocated array of `apartment` structures on success.
  *             NULL on failure or empty file (check `*array_size` and `errno`).
  *
  * @note       - File must contain tightly packed `apartment` structures with no padding.
  *             - Caller must `free()` the returned array after use.
  *             - Empty files return NULL with `*array_size = 0` (not an error).
  *             - Data validity (e.g., field alignment) is not checked beyond size constraints.
  */
 apartment * read_apartments_from_binary_file(const char *filename, size_t *array_size) {
     // Checking the passed arguments for validity.
     if (filename == NULL || array_size == NULL) {
         // If check was failed, terminating the function (setting `errno` to EINVAL, returning NULL).
         errno = EINVAL;
         return NULL;
     }
     
     size_t file_size;
     apartment *apartments = read_binary_file(filename, &file_size);
     
     // Trying to read data from file to array.
     if (apartments == NULL) {
         /*
          * If failure, terminating the function
          * (setting array size to file size (can be `(size_t)-1` or `0` depending on the presence of an error),
          * `errno` is already set, returning NULL).
          */
         *array_size = file_size;
         return NULL;
     }
     
     // Checking if the file size is correct (multiple of the `apartment` structure size).
     if (file_size % sizeof(apartment) != 0) {
         /*
          * If not, terminating the function
          * (setting array size to special `(size_t)-1` value, setting `errno` to EILSEQ and returning NULL)
          */
         *array_size = (size_t)-1;
         errno = EILSEQ;
         return NULL;
     }
     
     // Everything is ok.
     
     // The array size is the count of structures with the type of `apartment` in the file.
     *array_size = file_size / sizeof(apartment);
     
     // Returning the pointer to an array of apartments.
     return apartments;
 }
 
 /**
  * @brief      Finds an apartment by ID in a binary file and returns its details.
  *
  * @details    This function reads apartments from a binary file, searches for an apartment with the specified ID,
  *             and validates data integrity (e.g., uniqueness of the ID). Uses `predicate_search()` with `id_match`.
  *
  * @param[in]  filename   Path to the binary file (must not be NULL).
  * @param[in]  target_id  ID of the apartment to find (must be >0).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid arguments (`filename == NULL` or `target_id <= 0`).
  *               - EINVAL: Multiple apartments with the same ID found (data inconsistency).
  *               - Other codes: Errors from `read_apartments_from_binary_file()` or `predicate_search()`.
  *
  * @returns    - Valid `apartment` structure if found.
  *             - "Undefined" apartment (all fields set to invalid values) if:
  *                 - File is empty/corrupted.
  *                 - No matches.
  *                 - Data errors (e.g., duplicate IDs).
  *
  * @note       - The "undefined" apartment has `id = -1` and other fields zeroed/invalid.
  *             - Always check `errno` after calling this function to distinguish between errors and no-match cases.
  *             - File must contain properly serialized `apartment` structures.
  */
 apartment get_apartment_by_id(const char *filename, int target_id) {
     // The "undefined" value for `apartment` (invalid).
     apartment found_apt = { -1, "", 0, .0f, 0, .0f, false, { -1, -1, -1 } };
     
     // Checking if the passed arguments are valid.
     if (filename == NULL || target_id <= 0) {
         // If not, terminating the function (setting `errno` to EINVAL, returning "undefined" value).
         errno = EINVAL;
         return found_apt;
     }
     
     // Trying to get an information about all apartments from the file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         /*
          * If the apartments array is empty, the file is either empty or corrupted.
          * Returning "undefined" (`errno` is already set in case of error).
          */
         return found_apt;
     }
     
     // Getting the list of apartments with the same id.
     size_t apartments_with_the_target_id_count;
     apartment *apartments_with_the_target_id = predicate_search(apartments, apartments_count, sizeof(apartment),
                                                                 &apartments_with_the_target_id_count,
                                                                 (predicate_func_t)id_match, target_id);
     
     if (apartments_with_the_target_id == NULL) {
         // If the array of such apartments is empty, there could be an error, or no such apartments found.
         
         // Clearing the `apartments` array.
         free(apartments);
         
         // The array of found apartments is empty, returning the "undefined" apartment, `errno` is set in case of error.
         return found_apt;
     }
     
     if (apartments_with_the_target_id_count > 1) {
         // If the target id is not unique, it is an error.
         errno = EINVAL;
     } else {
         // There is only one element in the found apartments array.
         found_apt = apartments_with_the_target_id[0];
     }
     
     // Clearing dynamically allocated arrays.
     free(apartments_with_the_target_id);
     free(apartments);
     
     // Returning the found apartment.
     return found_apt;
 }
 
 /**
  * @brief      Checks if an apartment ID is unused in a binary file.
  *
  * @details    This function determines if the specified ID is not associated with any apartment in the file.
  *             It relies on `get_apartment_by_id()` to fetch data and `apartment_is_valid()` to validate results.
  *
  * @param[in]  filename  Path to the binary file (must not be NULL).
  * @param[in]  id        ID to check (must be >0).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: `filename` is NULL or `id <= 0`.
  *               - Other codes: Errors from `get_apartment_by_id()` (e.g., EIO, ENOMEM).
  *
  * @returns    - `true`:  ID is unused (no valid apartment found).
  *             - `false`: ID is used, arguments are invalid, or an error occurred.
  *
  * @note       - Check `errno` after calling:
  *                 - `true`:  `errno` unchanged (ID unused).
  *                 - `false`: `errno == EINVAL` (invalid args) or other errors (file issues).
  *             - If the file is empty/corrupted, returns `true` (ID considered unused).
  */
 bool apartment_id_is_unused(const char *filename, int id) {
     // Checking if the passed arguments are valid.
     if (filename == NULL || id <= 0) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning false).
         errno = EINVAL;
         return false;
     }
     
     // If the found by ID apartment is invalid, so there's no apartments with such ID in the file.
     return !apartment_is_valid(get_apartment_by_id(filename, id));
 }
 
 /**
  * @brief      Generates a unique apartment ID based on existing records in a binary file.
  *
  * @details    Reads all apartments from the file, sorts them by ID, and finds the smallest unused integer ID.
  *             If the file is empty, returns `1`. For non-empty files, checks sequentially for gaps in IDs.
  *
  * @param[in]  filename  Path to the binary file (must not be NULL).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: `filename` is NULL or invalid file content.
  *               - EOVERFLOW: Generated ID exceeds `INT_MAX`.
  *               - Other codes: Errors from `read_apartments_from_binary_file()` or `quicksort()`.
  *
  * @returns    - Unique ID (>=1) on success.
  *               - -1 on errors (invalid arguments, file corruption, or ID overflow).
  *
  * @note       - Depends on `read_apartments_from_binary_file()`, `quicksort()`, and `apt_id_compare()`.
  *             - If IDs are sparse (e.g., `[1, 3]`), returns the smallest missing value (`2` in this example).
  */
 int generate_unique_apartment_id(const char *filename) {
     // Checking the passed argument for validity.
     if (filename == NULL) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Trying to get an information about all apartments from the file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         // If the apartments array is empty, the file is either empty or damaged.
         if (apartments_count == (size_t)-1) {
             /*
              * If the apartments array size is `(size_t)-1`, the file is damaged. Terminating the function
              * (returning -1, `errno` is already set).
              */
             return -1;
         }
         
         // The file is empty, so the id of `1` will be pretty unique.
         return 1;
     }
     
     // Trying to find the smallest unused id value.
     
     // Trying to sort an array of apartments in ascending order of ids.
     if (quicksort(apartments, apartments_count, sizeof(apartment), apt_id_compare) == -1) {
         // If failed, terminating the function (clearing the `apartments` array, `errno` is already set, returning -1).
         free(apartments);
         return -1;
     }
     
     // Selecting a value for the id that is 1 greater than the current one.
     int new_unique_id = 1;
     for (size_t i = 0; i < apartments_count && apartments[i].id == new_unique_id; i++) {
         if (new_unique_id == INT_MAX) {
             /*
              * If there is no ids available (the limit of the number of apartments has been reached),
              * then terminating the function
              * (clearing the `apartments` array, setting `errno` to EOVERFLOW, returning -1).
              */
             free(apartments);
             errno = EOVERFLOW;
             return -1;
         }
         
         new_unique_id++;
     }
     
     // Everything is ok.
     
     // Clearing the `apartments` array.
     free(apartments);
     
     // Returning the generated unique id.
     return new_unique_id;
 }