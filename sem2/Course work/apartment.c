#include "apartment.h"

#include "auxiliary.h"

#include <stdlib.h>
#include <errno.h>

apartment_array apartment_array_create() {
	apartment_array apt_array;

	apt_array.capacity = 0;
	apt_array.size = 0;
	apt_array.data = NULL;

	return apt_array;
}

void apartment_array_push_back(apartment_array * apt_array, apartment apt) {
	if (apt_array->capacity == 0) {
		apt_array->data = (apartment *)calloc(++apt_array->capacity, sizeof(apartment));
		VERIFY(apt_array->data != NULL, NOTHING, ENOMEM);
	}

	if (apt_array->size == apt_array->capacity) {
		apartment * next_data_buffer = (apartment *)realloc(apt_array->data, (apt_array->capacity *= 2) * sizeof(apartment));
		if (next_data_buffer == NULL) {
			free(apt_array->data);
		}
		apt_array->data = next_data_buffer;
		VERIFY(apt_array->data != NULL, NOTHING, ENOMEM);
	}
	
	apt_array->data[apt_array->size++] = apt;
}

apartment_array get_database() {

}