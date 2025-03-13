#ifndef AUXILIARY_H
#define AUXILIARY_H 1

#include <stddef.h>

#define NOTHING (void)0
#define STRING_BUFFER_MAX_SIZE (size_t)(1024)
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define VERIFY(condition, return_value, errno_code) \
	if (!(condition)) {                         \
		errno = (errno_code);               \
		return (return_value);              \
        }                                           \
	NOTHING                                     \

char * substr(const char *, size_t, size_t);

double doublecmp(double, double);

#endif // AUXILIARY_H