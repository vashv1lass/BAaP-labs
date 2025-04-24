/**
 * @file  safeio.c
 * @brief Contains the functions (definitions) that may be needed when working with IO (input-output).
 */

 #include "safeio.h"

 #include <stdio.h>  // For `FILE`, `ferror()`, etc.
 #include <errno.h>  // For `errno` and its codes.
 #include <limits.h> // For `INT_MAX`.
 #include <stdarg.h> // For `va_list`, `va_start()`, etc.
 #include <string.h> // For `strlen()`.
 
 /**
  * @brief      Flushes the input stream by consuming characters until a newline or EOF.
  *
  * @details    This function reads and discards characters from the input stream until a newline (`\\n`)
  *             or end-of-file (EOF) is encountered. It first checks the validity of the stream and its
  *             error state, then performs flushing. If an error occurs during reading, the stream's error
  *             flags are reset using `clearerr()`.
  *
  * @param[in]  input_stream  The input stream to clear (must not be NULL).
  *
  * @errors     The function may fail and set `errno` to one of the following values:
  *               - EINVAL: \p input_stream is NULL.
  *               - EBADF: The stream had an existing error state before the function call.
  *               - Other errors: See `fgetc()` documentation for possible I/O errors.
  *
  * @returns    On success: 0.
  *             On failure: -1, with `errno` set to the error code.
  *
  * @note       This function is designed for text streams. For binary streams, the behavior is undefined
  *             due to reliance on `\\n` as a delimiter. Not thread-safe.
  */
 int flush_input_stream(FILE *input_stream) {
     // Checking the validity of the passed argument.
     if (input_stream == NULL) {
         // If invalid, then terminating the function (setting `errno` to EINVAL and returning -1).
         errno = EINVAL;
         return -1;
     }
     
     // Checking the validity of the file descriptor.
     if (ferror(input_stream) != 0) {
         // If invalid, then terminating the function (setting `errno` to EBADF and returning -1).
         errno = EBADF;
         return -1;
     }
     
     // Clearing the input stream.
     for (int c; (c = fgetc(input_stream)) != '\n' && c != EOF; );
     
     // Checking if there wasn't any error.
     if (ferror(input_stream) != 0) {
         // If was, then terminating the function (`errno` is already set, returning -1).
         clearerr(input_stream);
         return -1;
     }
     
     // Everything is ok, returning 0.
     return 0;
 }
 
 /**
  * @brief      Safely reads a line from the input stream with buffer overflow protection.
  *
  * @details    This function reads a line from the input stream into the provided buffer, ensuring
  *             no buffer overflow occurs. It checks argument validity, handles stream errors, and
  *             automatically flushes excess input if the buffer is too small. Trailing newline
  *             characters are removed. Uses `flush_input_stream()` to clear residual data on overflow.
  *
  * @param[out] buffer         The buffer to store the input line (must not be NULL).
  * @param[in]  buffer_size    The size of the buffer (must be > 0 and ≤ `INT_MAX`).
  * @param[in]  input_stream   The input stream to read from (must not be NULL).
  *
  * @errors     The function may fail and set `errno` to one of the following values:
  *               - EINVAL: \p buffer is NULL, \p buffer_size is 0, or \p input_stream is NULL.
  *               - EBADF: The stream had an existing error state before the call.
  *               - EOVERFLOW: \p buffer_size exceeds `INT_MAX`.
  *               - EMSGSIZE: Input line exceeds \p buffer_size (stream flushed automatically).
  *               - Other errors: See `fgets()` and `flush_input_stream()` documentation.
  *
  * @returns    On success: Pointer to \p buffer containing the input line (without trailing `\\n`).
  *             On failure: NULL, with `errno` set to the error code.
  *
  * @note       - Designed for text streams (relies on `\\n` as a delimiter).
  *             - Not thread-safe.
  *             - Truncates lines longer than \p buffer_size and flushes the stream.
  *             - EOF is treated as an error only if no data was read.
  */
 char * safe_fgets(char *buffer, size_t buffer_size, FILE *input_stream) {
     // Checking the validity of the passed arguments.
     if (buffer == NULL || buffer_size == 0 || input_stream == NULL) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning NULL).
         errno = EINVAL;
         return NULL;
     }
     
     // Checking the validity of the file descriptor.
     if (ferror(input_stream) != 0) {
         // If invalid, then terminating the function (setting `errno` to EBADF and returning NULL).
         errno = EBADF;
         return NULL;
     }
     
     // Checking if buffer_size does not exceed INT_MAX (`fgets()` receives `int`).
     if (buffer_size > (size_t)INT_MAX) {
         // If exceeds, terminating the function (setting `errno` to EOVERFLOW, returning NULL).
         errno = EOVERFLOW;
         return NULL;
     }
     
     // Saving the `errno` value before all the operations.
     int saved_errno = errno;
     
     // Trying to read data into a buffer.
     if (fgets(buffer, (int)buffer_size, input_stream) == NULL) {
         // If something went wrong, terminating the function (`errno` was set, returning NULL).
         
         if (feof(input_stream)) {
             // If there was an end of file, clearing the buffer and resetting errno.
             buffer[0] = '\0';
             errno = saved_errno;
         }
         
         return NULL;
     }
     
     size_t data_size = strlen(buffer);
     if (data_size > 0 && buffer[data_size - 1] != '\n') {
         // Buffer is full: flushing the input stream.
         if (flush_input_stream(input_stream) == 0) {
             // If flushing the buffer was successfully done, setting `errno` to EMSGSIZE.
             errno = EMSGSIZE;
         }
         
         // Returning NULL (because there was a buffer overflow).
         return NULL;
     }
     
     // Removing extraneous '\n'.
     if (data_size > 0) {
         buffer[data_size - 1] = '\0';
     }
     
     // Returning the buffer.
     return buffer;
 }
 
 /**
  * @brief      Safely parses formatted input from a stream with buffer overflow protection.
  *
  * @details    This function reads a line from the input stream using `safe_fgets()` (which prevents
  *             buffer overflow), then parses the data according to the specified format using `vsscanf()`.
  *             It checks argument validity, stream errors, and ensures `errno` is properly set for
  *             invalid formats or parsing failures.
  *
  * @param[in]  input_stream  The input stream to read from (must not be NULL).
  * @param[in]  format        The format string (must not be NULL, see `scanf()` syntax).
  * @param[in]  ...           Variable arguments matching the format specifiers.
  *
  * @errors     The function may fail and set `errno` to one of the following values:
  *               - EINVAL: \p input_stream or \p format is NULL, or the format is invalid.
  *               - EBADF: The stream had an existing error state before the call.
  *               - Other errors: See `safe_fgets()` and `vsscanf()` documentation.
  *
  * @returns    On success: The number of parsed values (≥0).
  *             On failure: EOF, with `errno` set to the error code.
  *
  * @note       - Relies on `safe_fgets()`, which has a fixed buffer size (`INPUT_BUFFER_MAX_SIZE`).
  *             - Designed for text streams. For binary streams, the behavior is undefined.
  *             - Not thread-safe.
  *             - Partial parsing (fewer values than expected) is not considered an error.
  *             - EOF is treated as an error only if parsing fails completely.
  */
 int safe_fscanf(FILE *input_stream, const char *format, ...) {
     // Checking the validity of the passed arguments.
     if (input_stream == NULL || format == NULL) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning EOF).
         errno = EINVAL;
         return EOF;
     }
     
     // Checking the validity of the file descriptor.
     if (ferror(input_stream) != 0) {
         // If invalid, then terminating the function (setting `errno` to EBADF and returning EOF).
         errno = EBADF;
         return EOF;
     }
     
     // Trying to read data into a buffer.
     char buffer[INPUT_BUFFER_MAX_SIZE];
     if (safe_fgets(buffer, INPUT_BUFFER_MAX_SIZE, input_stream) == NULL) {
         // If something went wrong, terminating the function (`errno` is set, returning EOF).
         return EOF;
     }
     
     // Saving the previous `errno` value.
     int saved_errno = errno;
     
     // Reading arguments from the buffer.
     va_list args;
     va_start(args, format);
     int parsed_count = vsscanf(buffer, format, args);
     va_end(args);
     
     // Checking if the parsing was successfully done.
     if (parsed_count == EOF && errno == saved_errno) {
         // If the format is incorrect and `errno` was not set, setting `errno` to EINVAL.
         errno = EINVAL;
     }
     
     // Returning the count of read values.
     return parsed_count;
 }
 
 /**
  * @brief      Safely parses formatted input from a string with argument validation.
  *
  * @details    This function parses data from the input string `str` according to the specified format
  *             using `vsscanf()`. It validates input arguments, checks for NULL pointers, and ensures
  *             `errno` is properly set for invalid parameters or parsing failures. The function does
  *             not modify `errno` on successful parsing unless explicitly set by `vsscanf()`.
  *
  * @param[in]  str     The input string to parse (must not be NULL).
  * @param[in]  format  The format string (must not be NULL, see `scanf()` syntax).
  * @param[in]  ...     Variable arguments matching the format specifiers.
  *
  * @errors     The function may fail and set `errno` to one of the following values:
  *               - EINVAL: \p str or \p format is NULL.
  *               - Other errors: Values set by `vsscanf()` (e.g., invalid format specifiers).
  *
  * @returns    On success: The number of parsed values (≥0).
  *             On failure: EOF, with `errno` set to the error code.
  *
  * @note       - Designed for text input; behavior with binary data is undefined.
  *             - Not thread-safe.
  *             - Partial parsing (fewer values than expected) is not considered an error.
  *             - Relies on standard `vsscanf()` behavior; refer to its documentation for parsing rules.
  */
 int safe_sscanf(const char *str, const char *format, ...) {
     // Checking the validity of the passed arguments.
     if (str == NULL || format == NULL) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning EOF).
         errno = EINVAL;
         return EOF;
     }
     
     // Reading arguments from the `str`.
     va_list args;
     va_start(args, format);
     int parsed_count = vsscanf(str, format, args);
     va_end(args);
     
     // Returning the count of read values.
     return parsed_count;
 }
 
 /**
  * @brief      Writes a string to a stream with immediate flushing and error checking.
  *
  * @details    This function writes the specified string to the output stream and immediately flushes
  *             the buffer to ensure data persistence. It performs validity checks on arguments and
  *             stream state, handles errors via `errno`, and guarantees atomicity for text streams.
  *             Uses `fputs()` for writing and `fflush()` for buffer clearance.
  *
  * @param[in]  str            The null-terminated string to write (must not be NULL).
  * @param[in]  output_stream  The output stream to write to (must not be NULL and in valid state).
  *
  * @errors     The function may fail and set `errno` to one of the following values:
  *               - EINVAL: \p str or \p output_stream is NULL.
  *               - EBADF: The stream had an existing error state before the call.
  *               - Other errors: See `fputs()` and `fflush()` documentation.
  *
  * @returns    On success: The result of `fputs()` (non-negative value).
  *             On failure: EOF, with `errno` set to the error code.
  *
  * @note       - Designed for text streams. For binary streams, use `fwrite()` instead.
  *             - Not thread-safe due to reliance on global `errno`.
  *             - Guarantees data is written unless an error occurs (flushes the buffer).
  */
 int instant_fputs(const char *str, FILE *output_stream) {
     // Checking the validity of the passed arguments.
     if (str == NULL || output_stream == NULL) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning EOF).
         errno = EINVAL;
         return EOF;
     }
     
     // Checking the validity of file descriptor.
     if (ferror(output_stream)) {
         // If invalid, then terminating the function (setting `errno` to EBADF and returning EOF).
         errno = EBADF;
         return EOF;
     }
     
     // Writing info into output stream.
     int fputs_result = fputs(str, output_stream);
     
     if (fputs_result != EOF) {
         // If fputs was successfully done, trying to clear the output buffer.
         if (fflush(output_stream) == EOF) {
             // If error, terminating the function (`errno` is already set, returning EOF).
             return EOF;
         }
     }
     
     // Returning the `fputs()` result.
     return fputs_result;
 }
 
 /**
  * @brief      Immediately writes formatted data to a stream with error handling.
  *
  * @details    The function performs the following steps:
  *               - Validates input arguments (non-NULL `output_stream` and `formatted_str`).
  *               - Uses `vfprintf()` to format and write data to the stream.
  *               - Flushes the output buffer via `fflush()` (only on successful write) to ensure immediate visibility.
  *               - Propagates I/O errors through `errno` for diagnostics.
  *             Designed for scenarios requiring atomic write-and-flush operations (e.g., real-time logging).
  *
  * @param[in]  output_stream   Output stream (e.g., `stdout`, file handle).
  * @param[in]  formatted_str   Format string with conversion specifiers (same as `printf()` family).
  * @param[in]  ...             Variable arguments matching the format specifiers.
  *
  * @errors     The function may fail and set `errno`:
  *               - `EINVAL`: Invalid argument (`output_stream`/`formatted_str` is NULL or invalid format).
  *               - `EIO`:    I/O error during `vfprintf()` or `fflush()`.
  *               - Other errors from underlying system calls (e.g., `ENOSPC` for disk full).
  *
  * @returns    On success: Number of characters written (as returned by `vfprintf()`).
  *             On failure: `EOF`, with `errno` set to indicate the error.
  */
 int instant_fprintf(FILE *output_stream, const char *formatted_str, ...) {
     // Checking the validity of the passed arguments.
     if (formatted_str == NULL || output_stream == NULL) {
         // If invalid, terminating the function (setting `errno` to EINVAL, returning EOF).
         errno = EINVAL;
         return EOF;
     }
     
     // Writing info into output stream.
     va_list args;
     va_start(args, formatted_str);
     int vfprintf_result = vfprintf(output_stream, formatted_str, args);
     va_end(args);
     
     if (vfprintf_result >= 0) {
         // If vfprintf was successfully done, trying to clear the output buffer.
         if (fflush(output_stream) == EOF) {
             // If error, terminating the function (`errno` is already set, returning EOF).
             return EOF;
         }
     }
     
     // Returning the `vfprintf()` result.
     return vfprintf_result;
 }