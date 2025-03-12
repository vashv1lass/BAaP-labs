#include "Person.h"

#include <stdio.h>

void printPerson(Person p) {
	printf("Фамилия: %s\n", p.surname);
	printf("Вес: %d\n", p.weight);
	printf("Рост: %d\n", p.height);
}