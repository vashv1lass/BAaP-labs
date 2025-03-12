#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "Person.h"

#include <stdbool.h>

typedef struct LinkedListNode {
	Person value;
	struct LinkedListNode *next;
} LinkedListNode;

typedef struct LinkedList {
	struct LinkedListNode *root;
} LinkedList;

void initializeLL(LinkedList *);

void insertLL(LinkedList *, Person);
void eraseLL(LinkedList *, Person);

LinkedListNode * findLL(LinkedList, int);

void printLL(LinkedList);

#endif // LINKED_LIST_H