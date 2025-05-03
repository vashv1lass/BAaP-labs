/**
 * @file  safeio.h
 * @brief Contains the functions (declarations) that may be needed when working with IO (input-output).
 */

 #ifndef SAFEIO_H
 #define SAFEIO_H 1
 
 #include <stdio.h> // For `FILE`.
 
 #include "auxiliary.h" // For `STRING_BUFFER_MAX_SIZE`.
 
 /// Maximum input buffer size.
 #define INPUT_BUFFER_MAX_SIZE STRING_BUFFER_MAX_SIZE
 
 // Flushes the input stream by consuming characters until a newline or EOF.
 int flush_input_stream(FILE *);
 
 // Safely reads a line from the input stream with buffer overflow protection.
 char * safe_fgets(char *, size_t, FILE *);
 // Safely parses formatted input from a stream with buffer overflow protection.
 int safe_fscanf(FILE *, const char *, ...);
 // Safely parses formatted input from a string with argument validation.
 int safe_sscanf(const char *, const char *, ...);
 
 // Writes a string to a stream with immediate flushing and error checking.
 int instant_fputs(const char *, FILE *);
 // Formatted output with immediate buffer flush (vfprintf + fflush equivalent).
 int instant_vfprintf(FILE *, const char *, va_list);
 // Immediately writes formatted data to a stream with error handling.
 int instant_fprintf(FILE *, const char *, ...);
 
 /// Works same as `safe_fgets()`, but the input stream is `stdin`.
 #define safe_gets(buffer, buffer_size) safe_fgets(buffer, buffer_size, stdin)
 /// Works same as `safe_fscanf()`, but the input stream is `stdin`.
 #define safe_scanf(format, ...)        safe_fscanf(stdin, format, __VA_ARGS__)
 
 /// Works same as `instant_fputs()`, but the output stream is `stdout`.
 #define instant_puts(str)                  instant_fputs(str, stdout)
 /// Works same as `instant_fprintf()`, but the output stream is `stdout`.
 #define instant_printf(formatted_str, ...) instant_fprintf(stdout, formatted_str, __VA_ARGS__)
 
 #endif // SAFEIO_H