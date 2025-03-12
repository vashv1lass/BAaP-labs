#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "LinkedList.h"
#include "Person.h"

#include <stdlib.h>

typedef struct HashTable {
	struct LinkedList *table;
	size_t size;
} HashTable;

void createHashTable(HashTable *, size_t);

void insertHashTable(HashTable *, Person);
void eraseHashTable(HashTable *, Person);

Person findHashTable(HashTable, int);

#endif // HASH_TABLE_H