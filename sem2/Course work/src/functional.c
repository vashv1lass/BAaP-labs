/**
 * @file  functional.c
 * @brief Contains the main functions (definitions) that is required for the given program functional.
 */

 #include "functional.h"

 #include <stdbool.h> // For `bool`, `true`, `false`.
 #include <stdarg.h>  // For `va_list`, `va_copy()`, etc.
 #include <stddef.h>  // For `NULL`, `size_t`, etc.
 #include <errno.h>   // For `errno` and its codes.
 #include <stdio.h>   // For `FILE`, `stdin`, etc.
 #include <stdlib.h>  // For `free()`, etc.
 
 #include "apartment.h"   // For `apartment`, apartment comparison functions, etc.
 #include "date.h"        // For `date`, `datecmp()`, etc.
 #include "binary_file.h" // For `create_binary_file()`, `append_to_binary_file()`, etc.
 #include "database.h"    // For `read_apartments_from_binary_file()`, `apartment_id_is_unused()`, etc.
 #include "algorithms.h"  // For `binary_search()`, `quicksort()`, etc.
 #include "safeio.h"      // For `instant_fputs()`, `safe_fscanf()`, etc.
 #include "auxiliary.h"   // For `predicate_func_t`.
 
 /**
  * @brief      Checks if the apartment fits the given price range and number of rooms.
  *
  * @param[in]  apt   Pointer to an apartment (must be valid, see `apartment_is_valid`).
  * @param[in]  args  List of arguments in the format: `double lower_cost, double upper_cost, int rooms_count`.
  *
  * @errors     Sets `errno = EINVAL` if `apt` is invalid or NULL.
  *
  * @returns    - true: The apartment meets the criteria.
  *             - false: Does not meet the criteria or there is an error (check `errno`).
  */
 static bool cost_range_and_rooms_count_match(const void *apt, va_list args) {
     // Checking the apartment pointer for validity.
     if (apt == NULL || !apartment_is_valid(*(const apartment *)apt)) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning false).
         errno = EINVAL;
         return false;
     }
     
     // Getting the necessary arguments via copying the variadic argument list.
     va_list args_copy;
     va_copy(args_copy, args);
     
     float target_cost_left_border = (float)va_arg(args_copy, double);
     float target_cost_right_border = (float)va_arg(args_copy, double);
     int target_rooms_count = va_arg(args_copy, int);
     
     va_end(args_copy);
     
     // Returning the comparison result.
     return target_cost_left_border <= ((const apartment *)apt)->cost &&
             ((const apartment *)apt)->cost <= target_cost_right_border &&
             ((const apartment *)apt)->rooms_count == target_rooms_count;
 }
 
 /**
  * @brief      Predicate function to check if an apartment is added after a target date and is not sold.
  *
  * @details    Validates the apartment pointer, compares its addition date with the target date (via `datecmp`),
  *             and checks if the apartment is marked as not sold. Intended for use with variadic search functions.
  *
  * @param[in]  apt    Pointer to the apartment to check (must not be NULL and must be valid).
  * @param[in]  args   Variadic arguments containing:
  *                    - `date target_date`: Date to compare against (must be a valid date).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid apartment pointer or invalid apartment data.
  *
  * @returns    - `true` if apartment's addition date is after `target_date` and apartment is not sold.
  *             - `false` otherwise (including validation failures).
  *
  * @note       - Uses `va_copy` to safely handle variadic arguments.
  *             - Expects `args` to contain exactly one `date` argument (undefined behavior otherwise).
  *             - Thread-unsafe due to `errno` usage.
  */
 static bool later_than_date_and_free_status_match(const void *apt, va_list args) {
     // Checking the apartment pointer for validity.
     if (apt == NULL || !apartment_is_valid(*(const apartment *)apt)) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning false).
         errno = EINVAL;
         return false;
     }
     
     // Getting the necessary arguments via copying the variadic argument list.
     va_list args_copy;
     va_copy(args_copy, args);
     
     date target_date = va_arg(args_copy, date);
     
     va_end(args_copy);
     
     // Returning the comparison result.
     return datecmp(((const apartment *)apt)->addition_date, target_date) == 1 && !((const apartment *)apt)->sold;
 }
 
 /**
  * @brief      Creates a file with overwrite capability in case of name conflicts.
  *
  * @details    The function attempts to create a file while saving the original `errno` state.
  *             If the file already exists (EEXIST error), it prompts the user to choose whether to overwrite it.
  *             Uses `create_binary_file()` for file creation and `safe_fscanf()` for input handling.
  *             Restores `errno` after resolving name conflicts.
  *
  * @param[in]  filename Name of the file to create (must not be NULL).
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: \p filename is NULL.
  *               - EEXIST: File exists and the user declined overwrite.
  *               - I/O errors: See `create_binary_file()` and `safe_fscanf()` documentation.
  *               - Invalid input: Non-numeric characters entered during the choice prompt.
  *
  * @returns    On success: 0 (creation/overwrite).
  *             On failure: -1 (check `errno` for details).
  *
  * @note       - Not thread-safe due to `errno` usage.
  */
 int create_file(const char *filename) {
     // Checking if the filename is valid.
     if (filename == NULL) {
         // If invalid, terminating the function (setting errno to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Saving the previous `errno` value.
     int saved_errno = errno;
     
     // Trying to create a new file without trying to overwrite the existing one.
     if (create_binary_file(filename, false) == -1) {
         // If failure, trying to find out the reason.
         if (errno != EEXIST) {
             // If the reason was not an attempt to create the file again, then the file cannot be created. Returning -1.
             return -1;
         }
         // Restoring the previous `errno` value.
         errno = saved_errno;
         
         // Giving the user a choice whether to overwrite the file or not.
         instant_fputs("Кажется, файл уже существует. Вы хотите его пересоздать? Введите в строке ниже опцию:\n"
                       "1 - Пересоздать файл.\n"
                       "Любое другое число - не пересоздавать.\n", stdout);
         int choice;
         if (safe_fscanf(stdin, "%d", &choice) != 1) {
             /*
              * If there was an error in `choice` input, then terminating the function
              * (`errno` is already set, returning -1).
              */
             return -1;
         }
         
         if (choice != 1 || (choice == 1 && create_binary_file(filename, true) == -1)) {
             /*
              * If user does not want to overwrite the existing file or there was an error, returning -1.
              * `errno` (if there was an error) is already set.
              */
             return -1;
         }
     }
     
     // Everything is ok, returning 0.
     return 0;
 }
 
 /**
  * @brief      Displays apartment data from a binary file to a specified output stream.
  *
  * @details    Reads apartment records from a binary file, validates the data, and prints
  *             them in a localized Russian format. Handles empty/corrupted files and ensures
  *             proper resource cleanup. Uses `read_apartments_from_binary_file()` and
  *             `fprint_apartment()` for core operations.
  *
  * @param[in]  output_stream Valid writable file stream (e.g., `stdout`).
  * @param[in]  filename      Path to the binary file containing apartment records.
  *
  * @errors     The function may fail and set `errno` to:
  *               - EINVAL: \p output_stream or \p filename is NULL.
  *               - EBADF:  Stream has errors (via `ferror()`).
  *               - Errors from `read_apartments_from_binary_file()` (e.g., ENOENT, EFBIG).
  *               - Errors from `fprint_apartment()` (e.g., EIO).
  *
  * @returns    0 on success.
  *             -1 on failure (invalid arguments, corrupted file, or I/O error).
  *
  * @note       - Output format (Russian localization):
  *                ```
  *                [ФАЙЛ ПУСТ]
  *                Идентификатор: %d\n
  *                Адрес: %s\n
  *                ... (other fields)
  *                Дата добавления в файл: DD.MM.YYYY
  *                ```
  *             - Prints two newlines (`\n\n`) between apartment entries for readability.
  *             - Automatically frees allocated memory for apartment data.
  *             - Empty files return 0 with "[ФАЙЛ ПУСТ]" message.
  */
 int view_file(FILE *output_stream, const char *filename) {
     // Checking if the filename is valid.
     if (output_stream == NULL || filename == NULL) {
         // If invalid, terminating the function (setting errno to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Checking output stream for validity.
     if (ferror(output_stream) != 0) {
         // If invalid, terminating the function (setting errno to EBADF, returning -1)
         errno = EBADF;
         return -1;
     }
     
     // Trying to get an information about all apartments from the file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         // If the apartments array is empty, the file is either empty or corrupted.
         if (apartments_count == (size_t)-1) {
             /*
              * If the apartments array size is `(size_t)-1`, the file is corrupted. Terminating the function
              * (returning -1, `errno` is already set).
              */
             return -1;
         }
         
         // The file is empty, printing the corresponding message.
         fputs("[ФАЙЛ ПУСТ]\n", output_stream);
         
         // Returning 0.
         return 0;
     }
     
     // Trying to print all the apartments.
     for (size_t i = 0; i < apartments_count; i++) {
         if (fprint_apartment(output_stream, apartments[i]) == -1) {
             // If an error occurred, terminating the function (clearing the apartments array and returning -1).
             free(apartments);
             return -1;
         }
         
         // Newline.
         fputs("\n", output_stream);
         
         if (i < apartments_count - 1) {
             // One more newline (if current apartment is not last).
             fputs("\n", output_stream);
         }
     }
     
     // Everything is ok, clearing the apartments array.
     free(apartments);
     
     // Returning 0.
     return 0;
 }
 
 /**
  * @brief      Adds an apartment record to a binary file.
  *
  * @details    Validates input arguments, generates a unique ID if necessary,
  *             and appends serialized data to the end of the file using `append_to_binary_file`.
  *             Ensures data integrity by checking ID availability via `apartment_id_is_unused`.
  *
  * @param[in]     filename  Path to the binary file (must not be NULL).
  * @param[in,out] apt       Pointer to the `apartment` structure. If `apt->id == 0` or the ID is occupied,
  *                          a new unique ID is generated.
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: `filename` or `apt` is NULL.
  *               - Errors from `generate_unique_apartment_id()` (e.g., EOVERFLOW).
  *               - File I/O errors from `append_to_binary_file()` (e.g., EIO).
  *
  * @returns    - 0: Success (record added, ID updated).
  *             - -1: Failure (invalid arguments, ID generation error, or file write error).
  *
  * @note       - The file must exist (uses `"r+b"` mode).
  *             - The `apt->id` field is overwritten if a new ID is generated.
  */
 int add_note_to_file(const char *filename, apartment *apt) {
     // Checking if the passed arguments are valid.
     if (filename == NULL || apt == NULL) {
         // If not, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Trying to generate a new id for an apartment if necessary.
     if (apt->id == 0 || !apartment_id_is_unused(filename, apt->id)) {
         if ((apt->id = generate_unique_apartment_id(filename)) == -1) {
             // If generation failure, then terminating the function (`errno` is already set, returning -1).
             return -1;
         }
     }
     
     // Returning the result of appending the apartment data to a file.
     return append_to_binary_file(filename, (const void *)apt, sizeof(apartment));
 }
 
 /**
  * @brief      Removes an apartment record from a binary file by ID.
  *
  * @details    Validates the ID, checks its existence via `apartment_id_is_unused`,
  *             and invokes `remove_from_binary_file` with the `id_match` predicate.
  *
  * @param[in]  filename     Path to the binary file (must not be NULL).
  * @param[in]  removing_id  ID of the apartment to remove (must be >0 and exist in the file).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: `filename` is NULL, `removing_id` <=0, or the ID does not exist.
  *               - EIO:    I/O errors during file operations.
  *               - ENOMEM: Insufficient memory to read the file.
  *               - Other codes: Errors from `apartment_id_is_unused()` or `remove_from_binary_file()`.
  *
  * @returns    - 0: Success (record removed).
  *             - -1: Failure (invalid arguments, ID not found, or file errors).
  *
  * @note       - Uses `id_match_wrapper` to extract `removing_id` from `va_list`.
  *             - Returns an error if the file is empty or corrupted.
  */
 int remove_note_from_file(const char *filename, int removing_id) {
     // Checking the passed arguments for validity.
     if (filename == NULL || removing_id <= 0) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Trying to check if the passed id is used in the file.
     int saved_errno = errno;
     if (apartment_id_is_unused(filename, removing_id)) {
         // If unused, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Checking if the `apartment_id_is_unused()` call was successful (errno was not overwritten).
     if (errno != saved_errno) {
         // If unsuccessful, terminating the function (errno is already set, returning -1).
         return -1;
     }
     
     // Returning the result of removing the apartment data from a file.
     return remove_from_binary_file(filename, sizeof(apartment), (predicate_func_t)id_match, removing_id);
 }
 
 /**
  * @brief      Edits an apartment record in a binary file by ID.
  *
  * @details    Removes the existing record with `editing_id` and appends a new one.
  *             Critical note: The operation is not atomic — partial failures may result in data loss.
  *
  * @param[in]  filename    Path to the binary file (must not be NULL).
  * @param[in]  editing_id  ID of the record to edit (must be >0 and exist in the file).
  * @param[in]  apt         New apartment data (must be valid).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: `filename` is NULL, `editing_id` <=0, or the ID does not exist.
  *               - EIO:    I/O errors during file operations.
  *               - ENOMEM: Insufficient memory to process the file.
  *               - Other codes: Errors from `apartment_id_is_unused`, `remove_note_from_file`, or `add_note_to_file`.
  *
  * @returns    - 0: Success (record updated).
  *             - -1: Failure (invalid arguments, ID not found, or partial write error).
  *
  * @note       - Data loss risk: If `remove_note_from_file` succeeds but `add_note_to_file` fails,
  *               the original record is deleted, and the new one is not saved.
  *             - Corrupted files trigger errors.
  */
 int edit_note_in_file(const char *filename, int editing_id, const apartment apt) {
     // Checking if the passed arguments are valid.
     if (filename == NULL || editing_id <= 0 || !apartment_is_valid(apt)) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Trying to check if the passed id is used in the file.
     int saved_errno = errno;
     if (apartment_id_is_unused(filename, editing_id)) {
         // If unused, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Checking if the `apartment_id_is_unused()` call was successful (errno was not overwritten).
     if (errno != saved_errno) {
         // If unsuccessful, terminating the function (errno is already set, returning -1).
         return -1;
     }
     
     // Removing the necessary data from the file and appending the new one. Returning the result.
     return remove_note_from_file(filename, editing_id) == -1 ||
            add_note_to_file(filename, (apartment *)&apt) == -1 ?
            -1 : 0;
 }
 
 /**
  * @brief      Searches for apartments in a binary file matching a specific cost.
  *
  * @details    Reads apartments from a binary file and performs a linear search for entries with cost equal to `cost`.
  *             Uses `linear_search` with `apt_cost_compare` for comparison. The result is a dynamically allocated array
  *             of matching apartments.
  *
  * @param[in]  filename                Path to the binary file (must not be NULL).
  * @param[in]  cost                    Target cost to search for (must be >0.0).
  * @param[out] found_apartments_count  Pointer to store the number of matches or error status (must not be NULL).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid arguments (NULL `filename`/`found_apartments_count`, or `cost <= 0.0`).
  *               - Other errors: see `read_apartments_from_binary_file()`, `linear search()`, `apt_cost_compare()`.
  *
  * @returns    - Pointer to the array of matching apartments (free with `free()`).
  *               - NULL: Error or no matches (check `*found_apartments_count` and `errno`).
  *
  * @note       - `found_apartments_count` is set to `(size_t)-1` on errors, `0` if no matches or file is empty.
  *             - Thread-unsafe due to `errno` usage.
  */
 apartment * linear_search_by_cost(const char *filename, float cost, size_t *found_apartments_count) {
     // Checking if the passed arguments are valid.
     if (filename == NULL || cost < COST_EPSILON || found_apartments_count == NULL) {
         // If invalid, terminating the function (setting errno to EINVAL, returning NULL).
         errno = EINVAL;
         return NULL;
     }
     
     // Trying to read apartments from file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         /*
          * If the apartments array is NULL, the file is empty or there is an error, terminating the function
          * (errno is set in case of error, `found_apartments_count` contains `0` (file is empty) or `(size_t)-1` (error).
          */
         *found_apartments_count = apartments_count;
         return NULL;
     }
     
     // Creating a valid sample with which apartments will be compared with.
     apartment apt_sample = { 0, "0", 1, AREA_EPSILON, 1, COST_EPSILON, false, { 1, 1, 1970 } };
     apt_sample.cost = cost;
 
     // Trying to find matching apartments.
     apartment *found_apartments = (apartment *)linear_search(
         apartments,
         apartments_count,
         (const void *)&apt_sample,
         sizeof(apartment),
         apt_cost_compare,
         found_apartments_count
     );
     
     // Clearing the apartments array.
     free(apartments);
     
     // Returning the found apartments array.
     return found_apartments;
 }
 
 /**
  * @brief      Performs a binary search for apartments with a specific room count in a binary file.
  *
  * @details    Reads apartments from the binary file, sorts them by room count, and performs a binary search
  *             to find all apartments matching the specified `rooms_count`. The result is a dynamically allocated
  *             array of matching apartments, with the count stored in `found_apartments_count`.
  *
  * @param[in]  filename                  Path to the binary file containing apartments (must not be NULL).
  * @param[in]  rooms_count               The number of rooms to search for (must be >=0).
  * @param[out] found_apartments_count    Pointer to store the number of found apartments or error status
  * 										 (must not be NULL).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid arguments (NULL pointers, `rooms_count < 0`).
  *               - Errors from `read_apartments_from_binary_file` (e.g., file not found, read error).
  *               - Errors from `quicksort` or `binary_search` (e.g., ENOMEM, EINVAL).
  *             The `found_apartments_count` is set to:
  *               - 0 if the file is empty or no matches found.
  *               - (size_t)-1 on error (check `errno` for details).
  *               - Number of matches on success.
  *
  * @returns    - Pointer to a dynamically allocated array of matching apartments (free with `free()`).
  *             - NULL if an error occurs or no matches found (check `*found_apartments_count` and `errno`).
  *
  * @note       - The function sorts the apartments in-memory; original file order is not preserved.
  *             - The caller is responsible for freeing the returned array.
  *             - Thread-unsafe due to `errno` usage and in-memory sorting.
  */
 apartment * binary_search_by_rooms_count(const char *filename, int rooms_count, size_t *found_apartments_count) {
     // Checking if the arguments are valid.
     if (filename == NULL || rooms_count <= 0 || found_apartments_count == NULL) {
         // If not, terminating the function (setting errno to EINVAL, returning NULL).
         errno = EINVAL;
         return NULL;
     }
     
     // Trying to read apartments from the file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         /*
          * If the apartments array is NULL, the file is empty or there is an error, terminating the function
          * (errno is set in case of error, `found_apartments_count` contains `0` (file is empty) or `(size_t)-1` (error).
          */
         *found_apartments_count = apartments_count;
         return NULL;
     }
     
     // Sorting an array by the rooms count for subsequent binary search.
     if (quicksort(apartments, apartments_count, sizeof(apartment), apt_rooms_count_compare) == -1) {
         // If the sort was failed, terminating the function (freeing `apartments`, errno is set, returning NULL).
         free(apartments);
         return NULL;
     }
     
     // Creating a valid sample with which apartments will be compared with.
     apartment apt_sample = { 0, "0", 1, AREA_EPSILON, 1, COST_EPSILON, false, { 1, 1, 1970 } };
     apt_sample.rooms_count = rooms_count;
     
     // Trying to find matching apartments.
     apartment *found_apartments = (apartment *)binary_search(
         apartments,
         apartments_count,
         (const void *)&apt_sample,
         sizeof(apartment),
         apt_rooms_count_compare,
         found_apartments_count
     );
     
     // Clearing the apartments array.
     free(apartments);
     
     // Returning the found apartments array.
     return found_apartments;
 }
 
 /**
  * @brief      Searches for apartments within a cost range and specific room count in a binary file.
  *
  * @details    Reads apartments from a binary file, filters them by cost range (`cost_left_border` to
  * 			   `cost_right_border`) and exact `rooms_count`, then sorts the results by addition date. The filtered and
  * 			   sorted apartments are returned as a dynamically allocated array.
  *
  * @param[in]  filename               Path to the binary file containing apartments (must not be NULL).
  * @param[in]  cost_left_border       Minimum cost boundary (must be > `COST_EPSILON` and <= `cost_right_border`).
  * @param[in]  cost_right_border      Maximum cost boundary (must be > `COST_EPSILON` and >= `cost_left_border`).
  * @param[in]  rooms_count            Number of rooms to match (must be >= 0).
  * @param[out] found_apartments_count Pointer to store the number of matches, 0, or error status (must not be NULL).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid arguments (NULL pointers, invalid cost boundaries, `rooms_count < 0`).
  *               - Errors from `read_apartments_from_binary_file` (e.g., file not found, read errors).
  *               - Errors from `predicate_search` (e.g., memory allocation failure).
  *               - Errors from `quicksort` (e.g., invalid comparator or memory issues).
  *             `found_apartments_count` is set to:
  *               - 0 if no matches or the file is empty.
  *               - `(size_t)-1` on error (check `errno`).
  *               - The number of matches on success.
  *
  * @returns    - Pointer to a dynamically allocated array of matching apartments (free with `free()`).
  *             - NULL if an error occurs or no matches are found (check `*found_apartments_count` and `errno`).
  *
  * @note       - Sorting is performed in-memory; the original file order is not preserved.
  *             - The caller must free the returned array to avoid memory leaks.
  *             - Thread-unsafe due to `errno` usage and in-memory sorting.
  */
 apartment * search_by_cost_range_and_rooms_count(const char *filename, float cost_left_border, float cost_right_border,
                                                  int rooms_count, size_t *found_apartments_count) {
     // Checking if the passed arguments are valid.
     if (filename == NULL || cost_left_border < COST_EPSILON || cost_right_border < COST_EPSILON ||
         cost_left_border > cost_right_border || rooms_count < 0 || found_apartments_count == NULL) {
         // If invalid, terminating the function (setting errno to EINVAL, returning NULL).
         errno = EINVAL;
         return NULL;
     }
     
     // Trying to read apartments from file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         /*
          * If the apartments array is NULL, the file is empty or there is an error, terminating the function
          * (errno is set in case of error, `found_apartments_count` contains `0` (file is empty) or `(size_t)-1` (error).
          */
         *found_apartments_count = apartments_count;
         return NULL;
     }
     
     // Trying to write all matching apartments to a file.
     apartment *matching_apartments = (apartment *)predicate_search(
         apartments,
         apartments_count,
         sizeof(apartment),
         found_apartments_count,
         (predicate_func_t)cost_range_and_rooms_count_match,
         cost_left_border,
         cost_right_border,
         rooms_count
     );
     
     // Clearing the apartments array.
     free(apartments);
     
     if (matching_apartments == NULL) {
         /*
          * If `matching_apartments` is empty, there are no matching apartments or there was an error,
          * terminating the function (errno is set in case of error, `found_apartments_count` contains 0 in case of
          * matches absence or (size_t)-1 in case of error, returning NULL).
          */
         return NULL;
     }
     
     // Trying to sort the array by addition date.
     if (quicksort(matching_apartments, *found_apartments_count, sizeof(apartment), apt_addition_date_compare) == -1) {
         // If sort failed, returning NULL (errno is set).
         return NULL;
     }
     
     // Everything is ok, returning the matches apartments array.
     return matching_apartments;
 }
 
 /**
  * @brief      Searches for apartments added after a specific date and not marked as sold.
  *
  * @details    Reads apartments from a binary file, filters them using a predicate that checks if the
  *             apartment's addition date is after `earliest_date` and if it is not sold. The result is
  *             dynamically allocated and sorted by addition date (via `predicate_search`).
  *
  * @param[in]  filename               Path to the binary file containing apartments (must not be NULL).
  * @param[in]  earliest_date          The earliest acceptable addition date (must be valid).
  * @param[out] found_apartments_count Pointer to store the number of matches or error status (must not be NULL).
  *
  * @errors     The function may set `errno` to:
  *               - EINVAL: Invalid arguments (NULL pointers, invalid `earliest_date`).
  *               - Errors from `read_apartments_from_binary_file` (e.g., file not found, read error).
  *               - Errors from `predicate_search` (e.g., ENOMEM, EINVAL).
  *             The `found_apartments_count` is set to:
  *               - 0 if the file is empty or no matches found.
  *               - (size_t)-1 on error (check `errno` for details).
  *               - Number of matches on success.
  *
  * @returns    - Pointer to a dynamically allocated array of matching apartments (free with `free()`).
  *             - NULL if an error occurs or no matches found (check `*found_apartments_count` and `errno`).
  *
  * @note       - The caller must free the returned array.
  *             - Thread-unsafe due to `errno` usage and in-memory operations.
  */
 apartment * search_newest_free_apartments(const char *filename, const date earliest_date,
                                           size_t *found_apartments_count) {
     // Checking if the passed arguments are valid.
     if (filename == NULL || !date_is_valid(earliest_date) || found_apartments_count == NULL) {
         // If invalid, terminating the function (setting errno to EINVAL, returning NULL).
         errno = EINVAL;
         return NULL;
     }
     
     // Trying to read apartments from file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         /*
          * If the apartments array is NULL, the file is empty or there is an error, terminating the function
          * (errno is set in case of error, `found_apartments_count` contains `0` (file is empty) or
          * `(size_t)-1` (error).
          */
         *found_apartments_count = apartments_count;
         return NULL;
     }
     
     // Trying to write all matching apartments to a file.
     apartment *matching_apartments = (apartment *)predicate_search(
             apartments,
             apartments_count,
             sizeof(apartment),
             found_apartments_count,
             (predicate_func_t)later_than_date_and_free_status_match,
             earliest_date
     );
     
     // Clearing the apartments array.
     free(apartments);
     
     // Everything is ok, returning the matching apartments array.
     return matching_apartments;
 }
 
 /**
  * @brief      Sorts apartments by cost in a binary file.
  *
  * @details    Reads all apartments from the file, performs in-memory quicksort by cost,
  *             and overwrites the original file. This operation is NOT atomic:
  *             partial writes or errors may corrupt the file. Original data is lost
  *             after `create_binary_file()` call, even if subsequent steps fail.
  *
  * @param[in]  filename  Path to the binary file (must not be NULL, must exist).
  *
  * @errors     Sets `errno` to:
  *               - EINVAL: `filename` is NULL.
  *               - Other errors from `quicksort()`, `create_binary_file()`, etc.
  *
  * @returns    - 0:  Success.
  *             - -1: Error (check `errno` for details).
  *
  * @warning    This function is NOT thread-safe and NOT crash-resistant.
  *             File corruption may occur if interrupted during execution.
  *             Always backup the file before calling.
  *
  * @note       - Uses `quicksort` with `apt_cost_compare` for sorting.
  *             - Overwrites the entire file contents.
  */
 int quicksort_by_cost(const char *filename) {
     // Checking if the filename is valid.
     if (filename == NULL) {
         // If not, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Reading the binary file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         // If the apartments array is empty, there was an error or the file is empty (errno is set, returning -1).
         return -1;
     }
     
     if ((quicksort(apartments, apartments_count, sizeof(apartment), apt_cost_compare) == -1) ||
         (create_binary_file(filename, true) == -1) ||
         (append_to_binary_file(filename, (const void *) apartments, apartments_count * sizeof(apartment)) == -1)) {
         /*
          * If something (sorting the apartments array, rewriting the sorted apartments into a file)
          * went wrong, terminating the function (clearing the apartments array, errno is set, returning -1).
          */
         free(apartments);
         return -1;
     }
     
     // Clearing the apartments array.
     free(apartments);
     
     // Everything is ok, returning 0.
     return 0;
 }
 
 /**
  * @brief      Sorts apartments by area in a binary file.
  *
  * @details    Reads all apartments from the file, performs in-memory selection sort by area,
  *             and overwrites the original file. This operation is NOT atomic:
  *             partial writes or errors may corrupt the file. Original data is lost
  *             after `create_binary_file()` call, even if subsequent steps fail.
  *
  * @param[in]  filename  Path to the binary file (must not be NULL, must exist).
  *
  * @errors     Sets `errno` to:
  *               - EINVAL: `filename` is NULL.
  *               - Other errors from `selection_sort()`, `create_binary_file()`, etc.
  *
  * @returns    - 0:  Success.
  *             - -1: Error (check `errno` for details).
  *
  * @warning    This function is NOT thread-safe and NOT crash-resistant.
  *             File corruption may occur if interrupted during execution.
  *             Always backup the file before calling.
  *
  * @note       - Uses `selection_sort` with `apt_area_compare` for sorting.
  *             - Overwrites the entire file contents.
  */
 int selection_sort_by_area(const char *filename) {
     // Checking if the filename is valid.
     if (filename == NULL) {
         // If not, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Reading the binary file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         // If the apartments array is empty, there was an error or the file is empty (errno is set, returning -1).
         return -1;
     }
     
     if ((selection_sort(apartments, apartments_count, sizeof(apartment), apt_area_compare)) ||
         (create_binary_file(filename, true) == -1) ||
         (append_to_binary_file(filename, (const void *)apartments, apartments_count * sizeof(apartment)) == -1)) {
         /*
          * If something (sorting the apartments array, rewriting the sorted apartments into a file)
          * went wrong, terminating the function (clearing the apartments array, errno is set, returning -1).
          */
         free(apartments);
         return -1;
     }
     
     // Clearing the apartments array.
     free(apartments);
     
     // Everything is ok, returning 0.
     return 0;
 }
 
 /**
  * @brief      Sorts apartments by addition date in a binary file.
  *
  * @details    Reads all apartments from the file, performs in-memory insertion sort by addition date,
  *             and overwrites the original file. This operation is NOT atomic:
  *             partial writes or errors may corrupt the file. Original data is lost
  *             after `create_binary_file()` call, even if subsequent steps fail.
  *
  * @param[in]  filename  Path to the binary file (must not be NULL, must exist).
  *
  * @errors     Sets `errno` to:
  *               - EINVAL: `filename` is NULL.
  *               - Other errors from `insertion_sort()`, `create_binary_file()`, etc.
  *
  * @returns    - 0:  Success.
  *             - -1: Error (check `errno` for details).
  *
  * @warning    This function is NOT thread-safe and NOT crash-resistant.
  *             File corruption may occur if interrupted during execution.
  *             Always backup the file before calling.
  *
  * @note       - Uses `insertion_sort` with `apt_area_compare` for sorting.
  *             - Overwrites the entire file contents.
  */
 int insertion_sort_by_addition_date(const char *filename) {
     // Checking if the filename is valid.
     if (filename == NULL) {
         // If not, terminating the function (setting `errno` to EINVAL, returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Reading the binary file.
     size_t apartments_count;
     apartment *apartments = read_apartments_from_binary_file(filename, &apartments_count);
     
     if (apartments == NULL) {
         // If the apartments array is empty, there was an error or the file is empty (errno is set, returning -1).
         return -1;
     }
     
     if ((insertion_sort(apartments, apartments_count, sizeof(apartment), apt_addition_date_compare)) ||
         (create_binary_file(filename, true) == -1) ||
         (append_to_binary_file(filename, (const void *)apartments, apartments_count * sizeof(apartment)) == -1)) {
         /*
          * If something (sorting the apartments array, rewriting the sorted apartments into a file)
          * went wrong, terminating the function (clearing the apartments array, errno is set, returning -1).
          */
         free(apartments);
         return -1;
     }
     
     // Clearing the apartments array.
     free(apartments);
     
     // Everything is ok, returning 0.
     return 0;
 }