/**
 * @file  logging.c
 * @brief Contains the log-function definitions.
 */

#include "logging.h"

#include <errno.h>  // For `errno`.
#include <stdio.h>  // For `FILE`, `fopen()`, etc.
#include <string.h> // For `strerror()`
#include <stdarg.h> // For `va_list`, `va_start()`, etc.

#include "safeio.h"     // For `instant_fputs()`, etc.
#include "apartment.h"  // For `apartment`, `fprint_apartment()`.
#include "functional.h" // For `view_file()`.

/**
 * @brief      Initializes the log file by opening it and writing a header.
 *
 * @details    This function opens a file with the name `LOG_FILE_NAME` in write mode ("w"), verifies the success of the
 *             operation, writes a startup message with a separator, and closes the file. If errors occur during opening
 *             or closing, it outputs messages to `stderr` via `instant_fputs()`. After completion, `errno` is reset to
 *             the old value.
 *
 * @errors     May output the following messages to `stderr`:
 *               - "Ошибка инициализации файла для логирования!\\n" if file opening fails.
 *               - "Ошибка закрытия файла для логирования!\\n" if closing fails (`fclose()` returns `EOF`).
 *
 * @note       - The file is overwritten on every call ("w" mode).
 *             - `errno` is reset to the old value at the end.
 *             - Not thread-safe (relies on global `errno`).
 *             - Uses `instant_fputs()` for writing, which guarantees immediate buffer flushing.
 */
void log_file_initialize() {
    // Saving the previous errno value.
    int saved_errno = errno;
    
    // Opening the file in "write" mode.
    FILE *log_fp = fopen(LOG_FILE_NAME, "w");
    
    // Checking if the file opening was successful.
    if (log_fp == NULL) {
        instant_fputs("Ошибка инициализации файла для логирования!\n", stderr);
        return;
    }
    
    instant_fputs(
        "Выполнена инициализация файла логирования.\n\n"
        "---------------------------------------------------------------\n\n",
        log_fp
    );
    
    // Closing the file.
    if (fclose(log_fp) == EOF) {
        instant_fputs("Ошибка закрытия файла для логирования!\n", stderr);
    }
    
    // Restoring the previous errno value.
    errno = saved_errno;
}

/**
 * @brief      Logs formatted message with error context to an open log stream.
 *
 * @details    This function writes a formatted message (using variable arguments) to a pre-opened log stream
 *             It validates input arguments and stream state, appends saved `errno` context if present,
 *             and ensures `errno` restoration. Designed for internal use with already opened log files.
 *
 * @param[in]  log_fp         Log file stream (must be valid, writable, and error-free).
 * @param[in]  formatted_msg  Format string for the message (must not be NULL).
 * @param[in]  output_args    Variable arguments list initialized with `va_start`.
 *
 * @errors     Outputs to `stderr` on failure:
 *               - "Ошибка. Аргумент(ы)..." if NULL arguments detected.
 *               - "Ошибка файлового дескриптора..." if stream has pre-existing errors (`ferror()` != 0).
 *               - "Ошибка записи..." if `instant_vfprintf()` fails (propagates original `errno`).
 *
 * @note       - Thread-unsafe: Uses non-reentrant `strerror()` and global `errno`.
 *             - Requires valid `va_list` initialized via `va_start` (does not call `va_end`).
 *             - Preserves original `errno` across execution (restored before return).
 *             - Caller must ensure stream is properly opened and formatted.
 */
static void log_current_state_opened_desc(FILE *log_fp, const char *formatted_msg, va_list output_args) {
    // Saving the previous errno value.
    int saved_errno = errno;
    
    // Checking if the passed arguments are valid.
    if (log_fp == NULL || formatted_msg == NULL) {
        // If not, outputting the error message and terminating the function.
        instant_fputs(
            "Ошибка. Аргумент(ы), переданные в функцию `log_current_state_opened_desc()` "
            "не являются валидными\n",
            stderr
        );
        errno = saved_errno;
        return;
    }
    
    // Checking the validity of file descriptor.
    if (ferror(log_fp) != 0) {
        // If invalid, outputting the error message, terminating the function.
        instant_fputs("Ошибка файлового дескриптора в функции `log_binary_file_state()`\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Outputting message.
    if (instant_vfprintf(log_fp, formatted_msg, output_args) == EOF) {
        instant_fputs("Ошибка записи о проблеме в файл для логирования!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    if (saved_errno != 0) {
        // Printing the error message if there is.
        instant_fprintf(
            log_fp,
            "\nКод возникшей ошибки: %d, расшифровка кода ошибки: %s",
            saved_errno,
            strerror(saved_errno)
        );
    }
    instant_fputs("\n\n", log_fp);
    
    // Restoring previous errno value.
    errno = saved_errno;
}

/**
 * @brief      Logs a formatted message with error context to the log file.
 *
 * @details    Opens the log file in append mode, writes a formatted message (with variadic arguments)
 *             and error details (if `errno` was set), then closes the file. Preserves the original `errno`
 *             value across execution. Handles argument validation, file I/O errors, and stream cleanup.
 *
 * @param[in]  formatted_msg  Format string for the log message (must not be NULL).
 * @param[in]  ...            Variable arguments matching the format string.
 *
 * @errors     Outputs to `stderr` on failure:
 *               - "Ошибка. Переданная в функцию..." if `formatted_msg` is NULL.
 *               - "Ошибка открытия файла..." if `fopen()` fails.
 *               - "Ошибка закрытия файла..." if `fclose()` fails.
 *
 * @note       - Thread-unsafe: Uses non-reentrant `strerror()`, global `errno`, and appends to a shared file.
 *             - File operations: Overwrites `LOG_FILE_NAME` in append mode ("a").
 *             - Restores `errno` to its pre-call value on exit.
 *             - Language inconsistency: Error messages are in Russian.
 */
void log_current_state(const char *formatted_msg, ...) {
    // Saving the previous errno value.
    int saved_errno = errno;
    
    // Validating the `formatted_msg` parameter.
    if (formatted_msg == NULL) {
        instant_fputs("Ошибка. Переданная в функцию логирования строка является нулевым указателем!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Opening the log file in "append" mode.
    FILE *log_fp = fopen(LOG_FILE_NAME, "a");
    
    // Checking if the file opening was successful.
    if (log_fp == NULL) {
        // If not, outputting the error message, setting errno to old value, terminating the function.
        instant_fputs("Ошибка открытия файла для логирования!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Getting output arguments.
    va_list output_args;
    va_start(output_args, formatted_msg);
    
    log_current_state_opened_desc(log_fp, formatted_msg, output_args);
    
    // Removing the output arguments.
    va_end(output_args);
    
    // Closing the file.
    if (fclose(log_fp) == EOF) {
        instant_fputs("Ошибка закрытия файла для логирования!\n", stderr);
    }
    
    // Restoring the previous errno value.
    errno = saved_errno;
}

/**
 * @brief      Logs apartment data with a formatted message to the log file.
 *
 * @details    Opens the log file in append mode, writes a formatted message (with variadic arguments),
 *             followed by apartment data. Handles argument validation, file I/O errors, and preserves
 *             the original `errno` value. Uses `fprint_apartment()` for structured data output.
 *
 * @param[in]  apt            The apartment structure to log.
 * @param[in]  formatted_msg  Format string for the log header (must not be NULL).
 * @param[in]  ...            Variable arguments matching the format string.
 *
 * @errors     Outputs to `stderr` on failure:
 *               - "Ошибка. Вместо сообщения..." if `formatted_msg` is NULL.
 *               - "Ошибка открытия файла..." if `fopen()` fails.
 *               - "Ошибка записи проблемы..." if `instant_vfprintf()` fails.
 *               - "Ошибка закрытия файла..." if `fclose()` fails.
 *
 * @note       - File operations: Appends to `LOG_FILE_NAME` ("a" mode).
 *             - Restores original `errno` on exit.
 */
void log_apartment(const apartment apt, const char *formatted_msg, ...) {
    // Saving the previous errno value.
    int saved_errno = errno;
    
    // Validating the `formatted_msg` parameter.
    if (formatted_msg == NULL) {
        instant_fputs("Ошибка. Вместо сообщения в функцию `log_apartment()` был передан нулевой указатель.\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Opening the log file in "append" mode.
    FILE *log_fp = fopen(LOG_FILE_NAME, "a");
    
    // Checking if the file opening was successful.
    if (log_fp == NULL) {
        instant_fputs("Ошибка открытия файла для логирования!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Getting output arguments.
    va_list output_args;
    va_start(output_args, formatted_msg);
    
    // Outputting message.
    if (instant_vfprintf(log_fp, formatted_msg, output_args) == EOF) {
        instant_fputs("Ошибка записи проблемы в файл для логирования!\n", stderr);
        
        fclose(log_fp);
        va_end(output_args);
        errno = saved_errno;
        return;
    }
    
    // Removing the output arguments.
    va_end(output_args);
    
    // Logging the apartment.
    instant_fputs("\n", log_fp);
    if (fprint_apartment(log_fp, apt) == -1) {
        instant_fputs("Ошибка вывода записи в файл для логирования!\n", log_fp);
    }
    instant_fputs("\n\n", log_fp);
    
    // Closing the file.
    if (fclose(log_fp) == EOF) {
        instant_fputs("Ошибка закрытия файла для логирования!\n", stderr);
    }
    
    // Restoring the previous errno message.
    errno = saved_errno;
}

/**
 * @brief      Logs binary file inspection details with a formatted message.
 *
 * @details    Validates input arguments, writes a formatted message to the log file, inspects the specified
 *             binary file using `view_file()`, and handles I/O errors. Preserves the original `errno` value
 *             throughout execution. Designed for appending diagnostic data to a log file.
 *
 * @param[in]  binary_file_name  Path to the binary file to inspect (must not be NULL).
 * @param[in]  formatted_msg     Format string for the log header (must not be NULL and match variadic arguments).
 * @param[in]  ...               Variadic arguments for the format string.
 *
 * @errors     Outputs to `stderr` or the log file:
 *               - "Ошибка. Один из аргументов..." if `binary_file_name` or `formatted_msg` is NULL.
 *               - "Ошибка открытия файла..." if `fopen(LOG_FILE_NAME)` fails.
 *               - "Ошибка записи сообщения..." if `instant_vfprintf()` fails.
 *               - "Ошибка просмотра файла..." if `view_file()` returns -1 (written to log file).
 *               - "Ошибка закрытия файла..." if `fclose()` fails.
 *
 * @note       - Thread-unsafe: Relies on global `errno` and non-atomic file operations.
 *             - Error preservation: Restores `errno` to its pre-call value on exit.
 */
void log_binary_file_state(const char *binary_file_name, const char *formatted_msg, ...) {
    // Saving the previous errno value.
    int saved_errno = errno;
    
    // Validating the `formatted_msg` parameter.
    if (binary_file_name == NULL || formatted_msg == NULL) {
        instant_fputs(
            "Ошибка. Один из аргументов, переданных в функцию `log_binary_file_state()` был передан как нулевой "
            "указатель.\n",
            stderr
        );
        errno = saved_errno;
        return;
    }
    
    // Opening the log file in "append" mode.
    FILE *log_fp = fopen(LOG_FILE_NAME, "a");
    
    // Checking if the file opening was successful.
    if (log_fp == NULL) {
        instant_fputs("Ошибка открытия файла для логирования!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Getting output arguments.
    va_list output_args;
    va_start(output_args, formatted_msg);
    
    // Outputting message.
    if (instant_vfprintf(log_fp, formatted_msg, output_args) == EOF) {
        instant_fputs("Ошибка записи сообщения в файл для логирования!\n", stderr);
        
        fclose(log_fp);
        va_end(output_args);
        errno = saved_errno;
        return;
    }
    
    // Removing the output arguments.
    va_end(output_args);
    
    // Logging the binary file.
    instant_fputs("\n\n", log_fp);
    if (view_file(log_fp, binary_file_name) == -1) {
        instant_fputs("Ошибка просмотра файла. Вероятно, файл повреждён.\n", log_fp);
    }
    instant_fputs("\n\n", log_fp);
    
    // Closing the file.
    if (fclose(log_fp) == EOF) {
        instant_fputs("Ошибка закрытия файла для логирования!\n", stderr);
    }
    
    // Restoring the previous errno value.
    errno = saved_errno;
}

/**
 * @brief      Outputs log file contents to a specified stream with error handling.
 *
 * @details    This function reads the log file and writes its contents character-by-character
 *             to the provided output stream. It validates input arguments and stream state, handles file I/O errors,
 *             and preserves the original `errno` value throughout execution.
 *
 * @param[in]  output_stream  The output stream to write to (must be valid and writable).
 *
 * @errors     Outputs to `stderr` on failure:
 *               - "Ошибка: аргумент функции..." if `output_stream` is NULL.
 *               - "Ошибка: файловый поток..." if `output_stream` has pre-existing errors (`ferror()` != 0).
 *               - "Ошибка открытия файла..." if `fopen(LOG_FILE_NAME)` fails.
 *               - "Ошибка вывода файла..." if writing to `output_stream` fails (`ferror()` != 0 after output).
 *               - "Ошибка закрытия файла..." if `fclose()` fails.
 *
 * @note       - Thread-unsafe: Relies on global `errno` and non-atomic file operations.
 */
void view_log_file(FILE *output_stream) {
    // Saving the errno value.
    int saved_errno = errno;
    
    // Validating the output_stream argument.
    if (output_stream == NULL) {
        instant_fputs("Ошибка: аргумент функции watch_log_file является нулевым указателем!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Checking the output stream for errors.
    if (ferror(output_stream) != 0) {
        instant_fputs("Ошибка: файловый поток, переданный в функцию watch_log_file повреждён!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Trying to open the file.
    FILE *log_fp = fopen(LOG_FILE_NAME, "r");
    if (log_fp == NULL) {
        instant_fputs("Ошибка открытия файла логирования!\n", stderr);
        errno = saved_errno;
        return;
    }
    
    // Outputting the file content char-by-char.
    for (int current_char; (current_char = fgetc(log_fp)) != EOF; ) {
        instant_fprintf(output_stream, "%c", current_char);
    }
    
    // Checking the output stream for errors.
    if (ferror(output_stream) != 0) {
        instant_fputs("Ошибка вывода файла в поток!\n", stderr);
    }
    
    // Trying to close the file.
    if (fclose(log_fp) == EOF) {
        instant_fputs("Ошибка закрытия файла для логирования!\n", stderr);
    }
    
    // Restoring errno.
    errno = saved_errno;
}