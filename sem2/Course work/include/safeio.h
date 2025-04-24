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
 // Immediately writes formatted data to a stream with error handling.
 int instant_fprintf(FILE *, const char *, ...);
 
 #endif // SAFEIO_H