#include "Date.h"

#include "Auxiliary.h"

#include <stdbool.h>
#include <wctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

bool dateFormatIsValid_(const wchar_t *dateStr) {
	if (wcslen(dateStr) == 10) {
		return (
				iswdigit(dateStr[0]) && iswdigit(dateStr[1]) &&
				dateStr[2] == L'.' &&
				iswdigit(dateStr[3]) && iswdigit(dateStr[4]) &&
				dateStr[5] == L'.' &&
				(iswdigit(dateStr[6]) && iswdigit(dateStr[7]) &&
				 iswdigit(dateStr[8]) && iswdigit(dateStr[9]))
		);
	}
	return false;
}

bool isLeap_(int year) {
	return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool dateIsValid_(Date date) {
	int daysCount[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (isLeap_(date.year)) {
		daysCount[1] = 29;
	}

	if (!(1 <= date.month && date.month <= 12)) {
		return false;
	}
	return date.day <= daysCount[date.month - 1];
}

Date getDate(const wchar_t *dateStr) {
	Date d;
	d.day = 0;
	d.month = 0;
	d.year = 0;

	if (!dateFormatIsValid_(dateStr)) {
		fwprintf(
			stderr,
			L"The string of \"%ls\" is not a date in DD.MM.YYYY format!\n",
			dateStr
		);
		errno = EINVAL;
		
		return d;
	}

	wchar_t *dayStr = subwcs(dateStr, 0, 2);
	wchar_t *monthStr = subwcs(dateStr, 3, 2);
	wchar_t *yearStr = subwcs(dateStr, 6, 4);

	d.day = (int)wcstol(dayStr, NULL, 10);
	d.month = (int)wcstol(monthStr, NULL, 10);
	d.year = (int)wcstol(yearStr, NULL, 10);

	free(dayStr);
	free(monthStr);
	free(yearStr);

	if (!dateIsValid_(d)) {
		fwprintf(stderr, L"The date of %ls does not exist!\n", dateStr);
		errno = EINVAL;

		d.day = 0;
		d.month = 0;
		d.year = 0;
	}

	return d;
}