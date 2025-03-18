#ifndef AUXILIARY_H
#define AUXILIARY_H 1

#include <errno.h> // errno, errno codes

// nothing
#define NOTHING (void)0
/*
 * checks if the condition is met. if it is, then program does nothing, if not, then function returns the return_value
 * and errno is set to errno_code
 */ 
#define VERIFY(condition, return_value, errno_code) \
	if (!(condition)) {                             \
		errno = (errno_code);                       \
		return (return_value);                      \
    }                                               \
	NOTHING

#endif // AUXILIARY_H