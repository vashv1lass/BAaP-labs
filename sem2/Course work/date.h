#ifndef DATE_H
#define DATE_H 1

typedef struct date {
	int day;
	int month;
	int year;
} date;

date get_date(const char *);

int datecmp(date, date);

#endif // DATE_H