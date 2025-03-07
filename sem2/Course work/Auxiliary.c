#include "Auxiliary.h"

#include <wchar.h>
#include <stdlib.h>
#include <limits.h>

wchar_t * subwcs(const wchar_t *str, size_t pos, size_t len) {
	len = MIN(len, wcslen(str) - pos);
	
	wchar_t *substring = malloc((len + 1) * sizeof(wchar_t));
	wcsncpy(substring, str + pos, len);
	substring[len] = '\0';
	
	return substring;
}

wchar_t * subwcsPtr(const wchar_t *first, const wchar_t *last) {
	if (last == NULL) {
		return subwcs(first, 0, SIZE_T_MAX);
	}
	return subwcs(first, 0, last - first + 1);
}