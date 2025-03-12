#include <stdio.h>
#include <stddef.h>
#include <locale.h>

#include "Person.h"
#include "HashTable.h"

int main(void) {
	setlocale(LC_ALL, "RU");
	size_t n;
	printf("Введите количество людей: ");
	scanf("%zu", &n);
	Person persons[n];
	for (size_t i = 0; i < n; i++) {
		printf("Введите фамилию %zu-го человека: ", i + 1);
		scanf("%s", persons[i].surname);
		printf("Введите вес %zu-го человека (кг): ", i + 1);
		scanf("%d", &persons[i].weight);
		printf("Введите рост %zu-го человека (см): ", i + 1);
		scanf("%d", &persons[i].height);
		puts("");
	}
	
	size_t m;
	printf("Введите размер хеш-таблицы: ");
	scanf("%zu", &m);
	HashTable personsTable;
	createHashTable(&personsTable, m);
	for (size_t i = 0; i < n; i++) {
		insertHashTable(&personsTable, persons[i]);
	}
	
	int weight;
	printf("Введите элемент, который хотите найти в хеш-таблице: \n");
	scanf("%d", &weight);
	Person found = findHashTable(personsTable, weight);
	
	printf("Массив:\n\n");
	for (size_t i = 0; i < n; i++) {
		printf("%zu-й человек:\n", i + 1);
		printPerson(persons[i]);
		puts("");
	}
	
	printf("Хеш-таблица:\n\n");
	for (size_t i = 0; i < m; i++) {
		printf("%zu:\n", i + 1);
		printLL(personsTable.table[i]);
		puts("");
	}
	
	if (found.weight != -1) {
		printf("Найденный человек:\n\n");
		printPerson(found);
		puts("");
	} else {
		printf("Человек с таким весом не найден!\n");
	}
	
	return 0;
}