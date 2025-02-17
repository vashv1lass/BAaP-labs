#include "Stack.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct Stack {
	int data;
	Stack *next;
} Stack;

Stack *push(Stack *root, int value) {
	Stack *newRoot = (Stack *)malloc(sizeof(Stack));
	
	newRoot->data = value;
	newRoot->next = root;
	
	return newRoot;
}

void setNext(Stack *s, Stack *newNext) {
	s->next = newNext;
}

Stack *getNext(Stack *s) {
	return s->next;
}

Stack *pop(Stack *root, int *prevRootValue) {
	if (isEmpty(root)) {
		printf("STACK_UNDERFLOW error. Aborted.");
		exit(STACK_UNDERFLOW);
	}
	Stack *previousRoot = root;
	if (prevRootValue != NULL) {
		*prevRootValue = previousRoot->data;
	}
	
	root = root->next;
	
	free(previousRoot);
	previousRoot = NULL;
	
	return root;
}

void clear(Stack **current) {
	Stack *prev = NULL;
	
	while (*current != NULL) {
		prev = *current;
		
		*current = (*current)->next;
		
		free(prev);
		prev = NULL;
	}
}

bool isEmpty(Stack *root) {
	return root == NULL;
}