#include "functional.h"

#include "auxiliary.h"

#include "apartment.h"

#include <stddef.h> // size_t

apartment_array find_by_cost(double cost) {
	apartment_array database = get_database();
	
	apartment_array found = apartment_array_create();
	for (size_t i = 0; i < database.size; i++) {
		if (doublecmp(database.data[i].cost, cost) == 0) {
			apartment_array_push_back(&found, database.data[i]);
		}
	}
	
	return found;
}

apartment_array find_by_rooms_count(int rooms_count) {
	apartment_array database = get_database();
	
	size_t left_border = 0;
	size_t right_border = database.size;
	while (left_border + 1 < right_border) {
		size_t middle = (left_border + right_border) / 2;
		if (database.data[middle].rooms_count <= rooms_count) {
			left_border = middle;
		} else {
			right_border = middle;
		}
	}
	
	apartment_array found = apartment_array_create();
	for (size_t i = left_border; i < database.size && database.data[i].rooms_count == rooms_count; i++) {
		apartment_array_push_back(&found, database.data[i]);
	}
	
	return found;
}