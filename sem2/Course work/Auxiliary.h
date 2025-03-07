#ifndef AUXILIARY_H
#define AUXILIARY_H 1

#include <wchar.h>
#include <stdlib.h>

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

wchar_t * subwcs(const wchar_t *, size_t, size_t);
wchar_t * subwcsPtr(const wchar_t *, const wchar_t *);

#endif // AUXILIARY_H