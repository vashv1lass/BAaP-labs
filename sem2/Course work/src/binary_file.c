#include "binary_file.h"

#include <stddef.h>  // For size_t.
#include <stdio.h>   // For `FILE` and its operations (`fopen(), `fclose()`, etc.).
#include <errno.h>   // For `errno` and its codes (EINVAL, EFBIG, etc.).
#include <stdint.h>  // For uintmax_t.
#include <stdbool.h> // For bool.
#include <stdlib.h>  // For `malloc()`, `free()`.
#include <stdarg.h>  // For `va_list` and its operations (`va_start()`, `va_end()`, etc.).

#include "auxiliary.h" // For predicate_func_t.

/**
 * @brief     Determines the size of a binary file in bytes via an opened FILE pointer.
 *
 * @details   The function calculates the binary file size by moving the file pointer to the end of the file
 *            and retrieving the position relative to the beginning.
 *            It performs checks for argument validity, handles errors by setting appropriate `errno` values,
 *            and returns a special value `(size_t)-1` in case of failure.
 *
 * @param[in] fp An opened FILE pointer in binary mode.
 *
 * @errors    The function may fail and set `errno` to one of the following values:
 *              - EINVAL: \p fp is NULL.
 *              - EFBIG: The binary file size exceeds the maximum value representable by `size_t`.
 *              - Other errors: see `fseek()`, `ftell()` documentation.
 *
 * @returns    On success: The size of the binary file in bytes.
 *             On failure: `(size_t)-1`, and `errno` contains the error code.
 */
static size_t binary_file_size(FILE *fp) {
	// Checking if the argument (fp) is valid.
	if (fp == NULL) {
		/*
		 * If not, interrupting the function
		 * (setting errno to EINVAL, returning the special "undefined" value `(size_t)-1`).
		 */
		errno = EINVAL;
		return (size_t)-1;
	}
	
	// Trying to get old pointer position in file
	long old_file_pos;
	if ((old_file_pos = ftell(fp)) == -1L) {
		/*
		 * If failure, interrupting the function
		 * (errno is already set, returning the special "undefined" value `(size_t)-1`).
		 */
		return (size_t)-1;
	}
	
	// Trying to seek the end of file.
	long file_end_pos;
	if (fseek(fp, 0, SEEK_END) != 0 || (file_end_pos = ftell(fp)) == -1L || fseek(fp, old_file_pos, SEEK_SET) != 0) {
		/*
		 * If something went wrong, interrupting the function
		 * (errno is already set, returning the special "undefined" value `(size_t)-1`).
		 */
		return (size_t)-1;
	}
	
	// Checking if file_end_pos can be converted from long type to size_t type (there is no overflows).
	if ((uintmax_t)file_end_pos > SIZE_MAX) {
		/*
		 * If not, interrupting the function (setting errno to EFBIG and returning the special "undefined" value
		 * `(size_t)-1`).
		 */
		errno = EFBIG;
		return (size_t)-1;
	}
	
	/*
	 * Everything is ok, returning the value of the end of the file position relative to its beginning in bytes
	 * (this will be the file size).
	 */
	return (size_t)file_end_pos;
}

/**
 * @brief      Creates a new empty binary file or overwrites an existing one.
 *
 * @details    The function creates an empty binary file. If the file already exists:
 *               - If \p overwrite = true, the file is truncated to zero length (overwritten).
 *               - If \p overwrite = false, the function fails with `errno = EEXIST`.
 *             It performs checks for argument validity, handles errors by setting appropriate `errno` values,
 *             and returns a special value of -1 in case of failure.
 *
 * @param[in]  filename  Name of the file to create (must not be NULL).
 * @param[in]  overwrite If true, overwrite existing file; if false, fail if file exists.
 *
 * @errors     The function may fail and set `errno` to one of the following values:
 *               - EINVAL: \p filename is NULL.
 *               - Other errors: See `fopen()` and `fclose()` documentation.
 *
 * @returns    On success: 0.
 *             On failure: -1, and `errno` contains the error code.
 */
int create_binary_file(const char *filename, bool overwrite) {
	// Checking if the argument (filename) is valid.
	if (filename == NULL) {
		// If not, interrupting the function (setting errno to EINVAL and returning -1).
		errno = EINVAL;
		return -1;
	}
	
	/*
	 * Trying to open the file in "wbx" mode (if the file must not be overwritten if it already exists)
	 * or in "wb" mode (if the file can be overwritten).
	 */
	FILE *fp = fopen(filename, !overwrite ? "wbx" : "wb");
	if (fp == NULL || fclose(fp) == EOF) {
		// If something went wrong, interrupting the function (errno is already set, returning -1).
		return -1;
	}
	
	// The file was successfully created, returning 0.
	return 0;
}

/**
 * @brief      Reads the contents of a binary file into a dynamically allocated buffer.
 *
 * @details    Opens the specified binary file in read mode ("rb"), determines its size,
 *             allocates a buffer of the appropriate size, reads the file's contents into the buffer
 *             and returns the buffer.
 *             If the file is empty, a NULL pointer is returned.
 *             Errors such as invalid arguments,
 *             file access issues (`fopen()`, `fseek()`, `ftell()`, `fread()`, `fclose()), memory allocation failures
 *             and file size limitations are reported via errno.
 *
 * @param[in]  filename   Path to a binary file to read (must not be NULL).
 * @param[out] file_size  Pointer to a variable that will receive the file size in bytes.
 *                        If an error occurs this is set to `(size_t)-1`.
 *                        If the file is empty, this is set to 0.
 *
 * @errors     EINVAL:       Invalid arguments (\p filename = NULL or \p file_size = NULL).
 *             ENOMEM:       Memory allocation failed (if no other errors occurred).
 *             Other errors: See `fopen()`, `fseek()`, `ftell()`, `fread()`, `fclose()`, `binary_file_size()` documentation.
 *
 * @returns    On success: A pointer to the allocated buffer containing the file's data.
 *                           - For a non-empty file: Buffer must be freed with `free()`.
 *                           - For an empty file:    NULL pointer, \p file_size is set to 0.
 *             On failure: NULL.
 *                           - `errno` is set to indicate the error.
 *                           - \p file_size is set to `(size_t)-1`
 */
void * read_binary_file(const char *filename, size_t *file_size) {
	// Checking if the arguments (filename and file_size) are valid.
	if (filename == NULL || file_size == NULL) {
		// If not, interrupting the function (setting errno to EINVAL and returning NULL).
		errno = EINVAL;
		
		if (file_size != NULL) {
			// If file_size is not NULL, assigning to file_size the special "undefined" value `(size_t)-1`.
			*file_size = (size_t)-1;
		}
		
		return NULL;
	}
	
	// Trying to open the file.
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL) {
		// If failure, interrupting the function (errno is already set, returning NULL).
		return NULL;
	}
	
	// Trying to get the file size.
	*file_size = binary_file_size(fp);
	if (*file_size == (size_t)-1) {
		/*
		 * If failure, interrupting the function
		 * (closing the file, setting file_size to special "undefined" value, setting errno to ENOMEM
	 	 * and returning NULL).
		 *
		 * But if the file closing error occurred, errno will be overwritten.
		 */
		fclose(fp);
		return NULL;
	}
	
	void *data_buffer = NULL;
	if (*file_size > 0) {
		// Trying to allocate memory for a file-sized buffer.
		data_buffer = malloc(*file_size);
		if (data_buffer == NULL) {
			/*
			 * If failure, interrupting the function
			 * (closing the file, setting file_size to special "undefined" value, setting errno to ENOMEM
			 * and returning NULL).
			 *
			 * But if the file closing error occurred, errno will be overwritten.
			 */
			*file_size = (size_t)-1;
			errno = ENOMEM;
			fclose(fp);
			return NULL;
		}
		
		// Trying to read data from the file into the buffer.
		if (fread(data_buffer, 1, *file_size, fp) != *file_size) {
			/*
			 * If failure (not all the data was written), interrupting the function (setting file_size to (size_t)-1,
			 * clearing the buffer, closing the file, errno is already set, returning NULL).
			 *
			 * But if the file closing error occurred, errno will be overwritten.
			 */
			*file_size = (size_t)-1;
			free(data_buffer);
			fclose(fp);
			return NULL;
		}
	}

	// Trying to close the file.
	if (fclose(fp) == EOF) {
		// If failure, interrupting the function (clearing the buffer, errno is already set, returning NULL).
		free(data_buffer);
		return NULL;
	}
	
	// The file was successfully read to memory, returning the buffer.
	return data_buffer;
}

/**
 * @brief     Appends data to the end of an existing binary file.
 *
 * @details   The function opens the specified binary file in read-write mode ("r+b"), seeks to the end of the file
 *            and appends the provided data there.
 *            Handles partial writes and ensures all data is appended.
 *            If an error occurs during appending, the file may be partially modified.
 *            It performs checks for argument validity, handles errors by setting appropriate `errno` values,
 *            and returns a special value -1 in case of failure.
 *
 * @param[in] filename  A null-terminated string containing the path to the file where the data is to be appended.
 * @param[in] data      Pointer to the data buffer to append.
 * @param[in] data_size Number of bytes to write.
 *
 * @errors    The function may fail and set `errno` to one of the following values:
 *            - EINVAL: Invalid arguments (NULL filename/data).
 *            - Other errors: see `fopen()`, `fseek()`, `fwrite()`, `fclose()` documentation.
 *
 * @returns   On success: 0.
 *            On failure: -1, and `errno` contains the error code.
 */
int append_to_binary_file(const char *filename, const void *data, size_t data_size) {
	// Checking if the arguments (filename and data) are valid.
	if (filename == NULL || data == NULL) {
		// If not, interrupting the function (setting errno to EINVAL and returning -1).
		errno = EINVAL;
		return -1;
	}
	
	// Trying to open the file.
	FILE *fp = fopen(filename, "r+b");
	if (fp == NULL) {
		// If failure, interrupting the function (errno is already set, returning -1).
		return -1;
	}
	
	// Trying to seek the end of file.
	if (fseek(fp, 0, SEEK_END) != 0) {
		/*
		 * If failure, interrupting the function (closing the file, errno is already set, returning -1).
		 *
		 * But if the file closing error occurred, errno will be overwritten.
		 */
		fclose(fp);
		return -1;
	}

	// Trying to append data to the file.
	size_t total_appended = 0;
	while (total_appended < data_size) {
		// Trying to write some part of data to a file by blocks.
		size_t currently_appended = fwrite((const char *)data + total_appended, 1, data_size - total_appended, fp);
		
		if (currently_appended == 0 || ferror(fp)) {
			/*
			 * If something went wrong, interrupting the function
			 * (closing the file, errno is already set, returning -1).
		     *
		     * But if the file closing error occurred, errno will be overwritten.
		     */
			fclose(fp);
			return -1;
		}
		
		total_appended += currently_appended;
	}
	
	// Trying to close the file.
	if (fclose(fp) == EOF) {
		// If failure, interrupting the function (errno is already set, returning -1).
		return -1;
	}

	// All the data were successfully appended, returning 0.
	return 0;
}

/**
 * @brief      Removes data from a binary file based on a predicate function.
 *
 * @details    The function reads the entire binary file into memory, filters out records that match the predicate,
 *             then overwrites the original file with the remaining records. Each record is assumed to be exactly
 *             \p data_size bytes long. The file is processed as follows:
 *               1. Original file content is read into memory.
 *               2. File is truncated to zero length.
 *               3. Non-matching records are appended back to the file.
 *             Partial writes during the append phase may leave the file in an inconsistent state if an error occurs.
 *
 * @param[in]  filename   Path to the binary file to modify (must not be NULL).
 * @param[in]  data_size  Size of each record in bytes (must be > 0).
 * @param[in]  predicate  Function pointer that determines if a record should be removed (return true = remove).
 * @param[in]  ...        Additional arguments to pass to the predicate function.
 *
 * @errors     The function may fail and set `errno` to:
 *               - EINVAL: Invalid arguments (\p filename = NULL, \p data_size = 0, or \p predicate = NULL).
 *               - Errors from `read_binary_file()`, `create_binary_file()`, `append_to_binary_file()`.
 *               - Any errors caused by the predicate function's implementation.
 *
 * @returns    On success: 0.
 *             On failure: -1, with `errno` set to indicate the error.
 *             If the file is empty: returns 0 (no action needed).
 */
int remove_from_binary_file(const char *filename, size_t data_size, predicate_func_t predicate, ...) {
	// Checking if the arguments (filename, data_size and predicate) are valid.
	if (filename == NULL || data_size == 0 || predicate == NULL) {
		// If not, interrupting the function (setting errno to EINVAL and returning -1).
		errno = EINVAL;
		return -1;
	}
	
	// Trying to read data from file into memory.
	size_t file_size;
	void *file_data = read_binary_file(filename, &file_size);
	if (file_data == NULL) {
		/*
		 * If the file is empty (file_size == 0), there is nothing to remove, returning 0, because this is not an error.
		 *
		 * If there was a failure, interrupting the function (errno is already set, returning -1).
		 */
		return file_size == 0 ? 0 : -1;
	}
	
	// Trying to clear the file.
	if (create_binary_file(filename, true) == -1) {
		// If failed, interrupting the function (clearing the file data buffer, errno is already set, returning -1).
		free(file_data);
		return -1;
	}
	
	// Getting the predicate arguments.
	va_list predicate_args;
	va_start(predicate_args, predicate);
	
	for (size_t i = 0; i < file_size / data_size; i++) {
		// Getting current file record.
		const void *current_data = (const void *)((uintptr_t)file_data + i * data_size);
		
		// Getting predicate arguments copy (protection against overwriting predicate arguments).
		va_list predicate_args_copy;
		va_copy(predicate_args_copy, predicate_args);
		
		// Does the current data match the predicate?
		bool matches_predicate = predicate(current_data, predicate_args_copy);
		
		// Removing predicate arguments copy
		va_end(predicate_args_copy);
		
		if (!matches_predicate) {
			// If current data do not match the predicate, trying to append it into a file.
			if (append_to_binary_file(filename, current_data, data_size) == -1) {
				/*
				 * If failure, interrupting the function
				 * (clearing the file data buffer, removing predicate arguments, errno is already set, returning -1).
				 */
				free(file_data);
				va_end(predicate_args);
				return -1;
			}
		}
	}
	
	// Removing predicate arguments.
	va_end(predicate_args);
	
	// Clearing the file data buffer.
	free(file_data);
	
	// Everything is ok, returning 0.
	return 0;
}