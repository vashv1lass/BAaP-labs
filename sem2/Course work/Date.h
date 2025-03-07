#ifndef DATE_H
#define DATE_H 1

#include <wchar.h>

typedef struct Date {
	int day;
	int month;
	int year;
} Date;

Date getDate(const wchar_t *);

#endif // DATE_H