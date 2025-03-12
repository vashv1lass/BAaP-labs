#include "LinkedList.h"

#include "Person.h"

#include <stdio.h>
#include <stdlib.h>

LinkedListNode * _Node_create(Person v) {
	LinkedListNode *node = (LinkedListNode *)malloc(sizeof(LinkedListNode));
	
	node->value = v;
	node->next = NULL;
	
	return node;
}

LinkedListNode * _Node_findPrev(LinkedList ll, Person v) {
	LinkedListNode *current = ll.root;
	if (current == NULL) {
		return NULL;
	}
	if (current->next == NULL || current->value.weight == v.weight) {
		return NULL;
	}
	if (current->next->value.weight == v.weight) {
		return current;
	}
	while ((current = current->next)->next != NULL) {
		if (current->next->value.weight == v.weight) {
			return current;
		}
	}
	return NULL;
}

LinkedListNode * _Node_findPrevByWeight(LinkedList ll, int w) {
	LinkedListNode *current = ll.root;
	if (current == NULL) {
		return NULL;
	}
	if (current->next == NULL || current->value.weight == w) {
		return NULL;
	}
	if (current->next->value.weight == w) {
		return current;
	}
	while ((current = current->next)->next != NULL) {
		if (current->next->value.weight == w) {
			return current;
		}
	}
	return NULL;
}

void initializeLL(LinkedList *ll) {
	ll->root = NULL;
}

void insertLL(LinkedList *ll, Person v) {
	LinkedListNode *current = ll->root;
	if (current == NULL) {
		ll->root = _Node_create(v);
	} else if (current->next == NULL) {
		ll->root->next = _Node_create(v);
	} else {
		while ((current = current->next)->next != NULL) {}
		current->next = _Node_create(v);
	}
}

void eraseLL(LinkedList *ll, Person v) {
	if (ll->root->value.weight == v.weight) {
		LinkedListNode *nodeToErase = ll->root;
		ll->root = ll->root->next;
		free(nodeToErase);
	} else {
		LinkedListNode *vPrev = _Node_findPrev(*ll, v);
		if (vPrev != NULL) {
			LinkedListNode *nodeToErase = vPrev->next;
			vPrev->next = vPrev->next->next;
			free(nodeToErase);
		}
	}
}

LinkedListNode * findLL(LinkedList ll, int weight) {
	if (ll.root == NULL) {
		return NULL;
	}
	if (ll.root->value.weight == weight) {
		return ll.root;
	}
	LinkedListNode *foundPrev = _Node_findPrevByWeight(ll, weight);
	if (foundPrev == NULL) {
		return foundPrev;
	}
	return foundPrev->next;
}

void printLL(LinkedList ll) {
	LinkedListNode *current = ll.root;
	if (current == NULL) {
		printf("Пусто!\n");
	}
	while (current != NULL) {
		printPerson(current->value);
		if ((current = current->next) != NULL) {
			puts("");
		}
	}
}