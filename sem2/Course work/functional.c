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
	
	int left_idx_border = -1;
	int right_idx_border = (int)database.size;
	while (left_idx_border + 1 < right_idx_border) {
		size_t middle_idx = ((size_t)left_idx_border + (size_t)right_idx_border) / 2;
		if (database.data[middle_idx].rooms_count < rooms_count) {
			left_idx_border = (int)middle_idx;
		} else {
			right_idx_border = (int)middle_idx;
		}
	}
	
	apartment_array found = apartment_array_create();
	for (size_t i = left_idx_border + 1; i < database.size && database.data[i].rooms_count == rooms_count; i++) {
		apartment_array_push_back(&found, database.data[i]);
	}
	
	return found;
}