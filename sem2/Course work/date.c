#include "date.h"

#include "auxiliary.h"

#include <stdlib.h> // strtol, free

date get_date(const char * date_str) {
	date d;
	
	char * day_str = substr(date_str, 0, 2);
	char * month_str = substr(date_str, 3, 2);
	char * year_str = substr(date_str, 6, 4);
	
	d.day = (int)strtol(day_str, NULL, 10);
	d.month = (int)strtol(month_str, NULL, 10);
	d.year = (int)strtol(year_str, NULL, 10);
	
	free(day_str);
	free(month_str);
	free(year_str);
	
	return d;
}

int datecmp(date lhs, date rhs) {
	if (lhs.year < rhs.year) {
		return -1;
	} else if (lhs.year > rhs.year) {
		return 1;
	}
	
	if (lhs.month < rhs.month) {
		return -1;
	} else if (lhs.month > rhs.month) {
		return 1;
	}
	
	if (lhs.day < rhs.day) {
		return -1;
	} else if (lhs.day > rhs.day) {
		return 1;
	}
	
	return 0;
}