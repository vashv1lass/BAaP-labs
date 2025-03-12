#include "HashTable.h"

#include "LinkedList.h"

#include <stdlib.h>

void createHashTable(HashTable *ht, size_t m) {
	ht->table = (LinkedList *)calloc(m, sizeof(LinkedList));
	for (size_t i = 0; i < m; i++) {
		initializeLL(&ht->table[i]);
	}
	
	ht->size = m;
}

void insertHashTable(HashTable *ht, Person p) {
	insertLL(&ht->table[p.weight % ht->size], p);
}

void eraseHashTable(HashTable *ht, Person p) {
	eraseLL(&ht->table[p.weight % ht->size], p);
}

Person findHashTable(HashTable ht, int weight) {
	LinkedListNode *found = findLL(ht.table[weight % ht.size], weight);
	if (found == NULL) {
		Person nil;
		nil.surname[0] = '\0';
		nil.height = -1;
		nil.weight = -1;
		return nil;
	}
	return findLL(ht.table[weight % ht.size], weight)->value;
}