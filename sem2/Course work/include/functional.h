/**
 * @file  functional.h
 * @brief Contains the main functions (declarations) that is required for the given program functional.
 */

 #ifndef FUNCTIONAL_H
 #define FUNCTIONAL_H 1
 
 #include <stdbool.h> // For `bool`.
 #include <stdarg.h>  // For `va_list`.
 #include <stdio.h>   // For `FILE`.
 
 #include "apartment.h" // For `apartment`.
 #include "date.h"      // For `date`.
 
 // Checks if the apartment fits the given price range and number of rooms.
 static bool cost_range_and_rooms_count_match(const void *, va_list);
 // Predicate function to check if an apartment is added after a target date and is not sold.
 static bool later_than_date_and_free_status_match(const void *, va_list);
 // Compares two apartments using the `apt_addition_date_compare()` function for sorting from new to old.
 static int apt_addition_date_compare_reversed(const void *, const void *);
 
 // Creates a file with overwrite capability in case of name conflicts.
 int create_file(const char *);
 // Displays apartment data from a binary file to a specified output stream.
 int view_file(FILE *, const char *);
 // Adds an apartment record to a binary file.
 int add_note_to_file(const char *, apartment *);
 // Removes an apartment record from a binary file by ID.
 int remove_note_from_file(const char *, int);
 // Edits an apartment record in a binary file by ID.
 int edit_note_in_file(const char *, int, apartment);
 
 // Searches for apartments in a binary file matching a specific cost.
 apartment * linear_search_by_cost(const char *, float, size_t *);
 // Performs a binary search for apartments with a specific room count in a binary file.
 apartment * binary_search_by_rooms_count(const char *, int, size_t *);
 // Searches for apartments within a cost range and specific room count in a binary file.
 apartment * search_by_cost_range_and_rooms_count(const char *, float, float, int , size_t *);
 // Searches for apartments added after a specific date and not marked as sold.
 apartment * search_newest_free_apartments(const char *, date, size_t *);
 
 // Sorts apartments by cost in a binary file.
 int quicksort_by_cost(const char *);
 // Sorts apartments by area in a binary file.
 int selection_sort_by_area(const char *);
 // Sorts apartments by addition date in a binary file.
 int insertion_sort_by_addition_date(const char *);
 
 #endif // FUNCTIONAL_H