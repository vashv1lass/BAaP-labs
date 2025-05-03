/**
 * @file  logging.h
 * @brief Contains the log-function declarations.
 */

 #ifndef LOGGING_H
 #define LOGGING_H 1
 
 #include "apartment.h"
 
 /// The name of the log file.
 #define LOG_FILE_NAME ("log.txt")
 
 // Initializes the log file by opening it and writing a header.
 void log_file_initialize();
 
 // Logs formatted message with error context to an open log stream.
 static void log_current_state_opened_desc(FILE *, const char *, va_list);
 
 // Logs a formatted message with error context to the log file.
 void log_current_state(const char *, ...);
 // Logs apartment data with a formatted message to the log file.
 void log_apartment(apartment, const char *, ...);
 // Logs binary file inspection details with a formatted message.
 void log_binary_file_state(const char *, const char *, ...);
 
 #endif // LOGGING_H