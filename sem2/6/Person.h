#ifndef PERSON_H
#define PERSON_H

#include "Auxiliary.h"

typedef struct Person {
	char surname[STRING_BUFFER_MAX_SIZE];
	
	int weight; // ключевое поле
	int height;
} Person;

void printPerson(Person);

#endif // PERSON_H