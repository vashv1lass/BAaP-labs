#include "auxiliary.h"

#include <stddef.h> // size_t
#include <stdlib.h> // calloc
#include <string.h> // strlen, strncpy
#include <errno.h>  // errno + errno codes
#include <math.h>   // fabs

char * substr(const char * str, size_t pos, size_t len) {
	const size_t str_length = strlen(str);
	
	len = MIN(len, str_length - pos);
	if (pos > str_length) {
		errno = ERANGE;
		return NULL;
	}
	
	char * substring = (char *)calloc(len + 1, sizeof(char));
	if (substring == NULL) {
		errno = ENOMEM;
		return NULL;
	}
	
	strncpy(substring, str + pos, len);
	substring[len] = '\0';
	
	return substring;
}

double doublecmp(double lhs, double rhs) {
	if (fabs(lhs - rhs) < 1e-4) {
		return .0;
	}
	return lhs - rhs;
}