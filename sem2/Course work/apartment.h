#ifndef APARTMENT_H
#define APARTMENT_H 1

#include "auxiliary.h"

#include <stdbool.h> // bool, true, false
#include <stddef.h>  // size_t

#include "date.h"

typedef struct apartment {
	int id;
	
	char address[STRING_BUFFER_MAX_SIZE];
	
	int rooms_count;
	double area;
	int floor;
	
	double cost;
	
	bool sold;
	
	date date_of_addition;
} apartment;

// TODO: добавить чтение данных о текущей квартире из бинарного файла
apartment get_next_apartment(/* TODO: добавить аргументы */);

typedef struct apartment_array {
	size_t size;
	size_t capacity;
	
	apartment * data;
} apartment_array;

apartment_array apartment_array_create();

void apartment_array_push_back(apartment_array *, apartment);

// TODO: добавить чтение базы данных из бинарного файла
apartment_array get_database(/* TODO: добавить аргументы */);

#endif // APARTMENT_H