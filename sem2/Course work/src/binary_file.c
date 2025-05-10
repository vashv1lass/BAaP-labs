/**
 * @file  binary_file.c
 * @brief Contains the basic functions (definitions) that may be needed when working with binary files.
 */

#include "binary_file.h"

#include <stdio.h>   // For `FILE`, etc.
#include <errno.h>   // For `errno` and its codes.
#include <stdint.h>  // For `uintmax_t`.
#include <stdbool.h> // For `bool`, `true`, `false`.
#include <stdlib.h>  // For `malloc()`, `free()`, etc.
#include <stdarg.h>  // For `va_list` `va_start()`, etc.

#include "auxiliary.h" // For `predicate_func_t`.

/**
 * @brief      Determines the size of a binary file by seeking to its end.
 *
 * @details    This function calculates the file size by moving the file pointer to the end and restoring
 *             the original position. It validates the input stream, checks for errors, and handles
 *             overflow conditions. Uses `fgetpos()`, `fseek()`, and `ftell()` for standard compliance.
 *
 * @param[in]  fp Valid pointer to an open binary file stream (must not be NULL).
 *
 * @errors     The function may fail and set `errno` to:
 *               - EINVAL: \p fp is NULL.
 *               - EBADF:  Stream has an error flag set (see `ferror()`).
 *               - EFBIG:  File size exceeds `SIZE_MAX` (overflow).
 *               - Errors from `fseek()`/`ftell()` (e.g., unsupported stream type, I/O error).
 *
 * @returns    File size in bytes on success.
 *             `(size_t)-1` on failure (check `errno` for details).
 *
 * @note       - The file must be opened in binary mode (`"rb"`). Text mode may cause incorrect results
 *                due to newline translations (e.g., on Windows).
 *             - Limited by `LONG_MAX` (use `fgetpos()` directly for large files >2GB on 32-bit systems).
 *             - Not thread-safe due to reliance on `errno`.
 *             - Fails on non-seekable streams (e.g., pipes, sockets).
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
    
    // Checking if the file descriptor (fp) is valid.
    if (ferror(fp) != 0) {
        // If not, terminating the function (setting `errno` to EBADF, returning `(size_t)-1`).
        errno = EBADF;
        return (size_t)-1;
    }
    
    // Trying to get old pointer position in file and moving the pointer to the end of file.
    fpos_t old_file_pos;
    if (fgetpos(fp, &old_file_pos) == -1 || fseek(fp, 0, SEEK_END) == -1) {
        // If something went wrong, terminating the function (errno is set, returning `(size_t)-1`).
        return (size_t)-1;
    }
    
    // Trying to get the file end position.
    long file_end_pos = ftell(fp);
    if (file_end_pos == -1L) {
        // If failed, setting the pointer to the old position and terminating the function (returning `(size_t)-1`).
        fsetpos(fp, &old_file_pos);
        return (size_t)-1;
    }
    
    // Trying to set the pointer to the old position.
    if (fsetpos(fp, &old_file_pos) != 0) {
        // If failed, terminating the function (errno is set, returning `(size_t)-1`.
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
 * @brief      Checks if a binary file exists by attempting to open it.
 *
 * @details    This function verifies the existence of a binary file by opening it in "rb" mode.
 *             It handles argument validity, saves/restores the `errno` state, and ensures the
 *             file is properly closed after the check. Uses `fopen()` and `fclose()` for cross-platform compatibility.
 *
 * @param[in]  filename The name of the file to check (must not be NULL).
 *
 * @errors     The function may fail and set `errno` to one of the following values:
 *               - EINVAL: \p filename is NULL.
 *               - Other errors: See `fopen()` and `fclose()` documentation (e.g., permission issues).
 *
 * @returns    `true` if the file exists and is accessible.
 *             `false` if the file does not exist or an error occurs (check `errno` for details).
 *
 * @note       - Designed for binary files but works with text files as well.
 *             - Not thread-safe due to reliance on `errno`.
 */
bool binary_file_exists(const char *filename) {
    // Checking if the argument (filename) is valid.
    if (filename == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning -1).
        errno = EINVAL;
        return false;
    }
    
    // Saving the previous `errno` value.
    int saved_errno = errno;
    
    // Trying to open the file in "rb" mode.
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL || fclose(fp) == EOF) {
        // If something went wrong, interrupting the function (errno is already set, returning false).
        if (errno == ENOENT) {
            // If the error reason was the file absence, setting `errno` to the saved value.
            errno = saved_errno;
        }
        
        return false;
    }
    
    // The file was successfully opened and closed, returning true, because it existed.
    return true;
}

/**
 * @brief      Creates a binary file with optional overwrite control.
 *
 * @details    This function creates a binary file or checks for existence before overwriting.
 *             Uses `binary_file_exists()` to verify file presence when `overwrite = false`,
 *             and `fopen("wb")` to create/truncate the file. Handles errors via `errno`.
 *
 * @param[in]  filename  Name of the file to create (must not be NULL).
 * @param[in]  overwrite If true, overwrite existing file; if false, fail if file exists.
 *
 * @errors     The function may fail and set `errno` to one of the following values:
 *               - EINVAL: \p filename is NULL.
 *               - EEXIST: File exists and `overwrite = false`.
 *               - Other errors: See `fopen()`, `fclose()`, and `binary_file_exists()` documentation.
 *
 * @returns    On success: 0.
 *             On failure: -1, with `errno` set to the error code.
 *
 * @note       - Vulnerable to race conditions between existence check and file creation.
 *             - Not thread-safe due to reliance on `binary_file_exists()` and global `errno`.
 *             - Uses `"wb"` mode: creates/truncates the file.
 */
int create_binary_file(const char *filename, bool overwrite) {
    // Checking if the argument (filename) is valid.
    if (filename == NULL) {
        // If not, interrupting the function (setting errno to EINVAL and returning -1).
        errno = EINVAL;
        return -1;
    }
    
    // If the file already exists, terminating the function (setting errno to EEXIST, returning -1).
    if (!overwrite && binary_file_exists(filename)) {
        errno = EEXIST;
        return -1;
    }
    
    // Trying to open the file in "wb" mode.
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL || fclose(fp) == EOF) {
        // If something went wrong, interrupting the function (errno is already set, returning -1).
        return -1;
    }
    
    // The file was successfully created, returning 0.
    return 0;
}

/**
 * @brief      Reads a binary file into dynamically allocated memory.
 *
 * @details    Opens a binary file, determines its size via `binary_file_size()`, allocates a buffer,
 *             and reads the entire content. Handles resource cleanup and error propagation. Uses
 *             `malloc()` for buffer allocation and `fread()` for data transfer.
 *
 * @param[in]  filename  Name of the file to read (must not be NULL).
 * @param[out] file_size Pointer to store the file size (must not be NULL). Set to `(size_t)-1` on failure.
 *
 * @errors     The function may fail and set `errno` to:
 *               - EINVAL: \p filename or \p file_size is NULL.
 *               - ENOMEM: Memory allocation failed.
 *               - Errors from `fopen()` (e.g., ENOENT), `binary_file_size()` (e.g., EFBIG), or `fread()` (e.g., EIO).
 *               - Errors from `fclose()` (overwrites previous errors).
 *
 * @returns    Pointer to the allocated buffer containing file data on success.
 *             NULL on failure (check `errno` and `*file_size` for details).
 *
 * @note       - The file must be a regular binary file (not a pipe/socket).
 *             - Caller must `free()` the returned buffer after use.
 *             - Returns NULL for empty files (0 bytes) but sets `*file_size = 0`.
 *             - Limited by `LONG_MAX` (use `fgetpos()` for large files >2GB on 32-bit systems).
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
        /*
         * If failure, interrupting the function
         * (setting file_size to special "undefined" value, errno is already set and returning NULL).
         */
        *file_size = (size_t)-1;
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
 * @brief      Appends binary data to a file.
 *
 * @details    Opens the file in `"r+b"` mode (binary read/write without truncation), seeks to the end,
 * 			   and appends data using `fwrite()`. Ensures full data write by checking `fwrite()` return values
 * 			   and handles errors via `errno`.
 *
 * @param[in]  filename   Path to the binary file (must not be NULL).
 * @param[in]  data       Pointer to the data buffer to append (must not be NULL).
 * @param[in]  data_size  Size of the data buffer in bytes (must be >0).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: `filename` or `data` is NULL.
 *               - EIO:    I/O error during file operations (e.g., `fwrite()` failure).
 *               - Other codes: Errors from `fopen()`, `fseek()`, or `fclose()`.
 *
 * @returns      - 0:  Success (data fully appended).
 *               - -1: Failure (invalid arguments, I/O error, or partial write).
 *
 * @note       - Partial writes are treated as errors (e.g., disk full).
 *             - `fclose()` errors are propagated, but may overwrite prior `errno` values.
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
 * @brief      Removes records from a binary file based on a predicate.
 *
 * @details    Reads the file into memory, validates data integrity, removes records matching the predicate,
 *             and overwrites the file. Uses `data_size` to ensure the file structure is valid.
 *
 * @param[in]  filename  Path to the binary file (must not be NULL).
 * @param[in]  data_size Size of one record in bytes (must be >0 and divide `file_size` evenly).
 * @param[in]  predicate Predicate function to filter records (must not be NULL).
 * @param[in]  ...       Variable arguments for the predicate (passed via `va_list`).
 *
 * @errors     The function may set `errno` to:
 *               - EINVAL: `filename`/`predicate` is NULL, `data_size` == 0,
 *                        or `file_size` is not a multiple of `data_size` (invalid file format).
 *               - EIO:    I/O errors during file operations.
 *               - ENOMEM: Insufficient memory to read the file.
 *               - Other codes: Errors from `predicate` or dependent functions.
 *
 * @returns    - 0: Success (file updated).
 *             - -1: Failure (invalid arguments, corrupted file, write errors).
 *
 * @note       - The file is overwritten in-place. Partial writes may result in data loss.
 *             - `data_size` must match the size of the record structure (POD type).
 *             - `fclose()` errors in `append_to_binary_file()` may overwrite `errno`.
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
    
    // Checking if the `data_size` is a multiplier of file_size.
    if (file_size % data_size != 0) {
        // If not, terminating the function (setting `errno` to EILSEQ, clearing the data buffer, returning -1).
        errno = EINVAL;
        free(file_data);
        return -1;
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
        
        int saved_errno = errno;
        
        // Getting predicate arguments copy (protection against overwriting predicate arguments).
        va_list predicate_args_copy;
        va_copy(predicate_args_copy, predicate_args);
        
        // Does the current data match the predicate?
        bool matches_predicate = predicate(current_data, predicate_args_copy);
        
        // Removing predicate arguments copy
        va_end(predicate_args_copy);
        
        if (errno != saved_errno) {
            /*
             * If predicate match check was failed, interrupting the function
             * (clearing the file data buffer, removing predicate arguments, errno is already set, returning -1).
             */
            va_end(predicate_args);
            free(file_data);
            return -1;
        }
        
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