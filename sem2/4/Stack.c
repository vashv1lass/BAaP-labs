#include "Stack.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct CharStack {
	char data;
	CharStack *next;
} CharStack;

CharStack *pushChar(CharStack *root, char value) {
	CharStack *newRoot = (CharStack *)malloc(sizeof(CharStack));
	
	newRoot->data = value;
	newRoot->next = root;
	
	return newRoot;
}

CharStack *popChar(CharStack *root, char *prevRootValue) {
	if (charStackIsEmpty(root)) {
		printf("STACK_UNDERFLOW error. Aborted.");
		exit(STACK_UNDERFLOW);
	}
	CharStack *previousRoot = root;
	if (prevRootValue != NULL) {
		*prevRootValue = previousRoot->data;
	}
	
	root = root->next;
	
	free(previousRoot);
	previousRoot = NULL;
	
	return root;
}

char getTopChar(const CharStack *root) {
	return root->data;
}

void clearCharStack(CharStack **current) {
	CharStack *prev = NULL;
	
	while (*current != NULL) {
		prev = *current;
		
		*current = (*current)->next;
		
		free(prev);
		prev = NULL;
	}
}

bool charStackIsEmpty(CharStack *root) {
	return root == NULL;
}

typedef struct FloatStack {
	float data;
	FloatStack *next;
} FloatStack;

FloatStack *pushFloat(FloatStack *root, float value) {
	FloatStack *newRoot = (FloatStack *)malloc(sizeof(FloatStack));
	
	newRoot->data = value;
	newRoot->next = root;
	
	return newRoot;
}

FloatStack *popFloat(FloatStack *root, float *prevRootValue) {
	if (floatStackIsEmpty(root)) {
		printf("STACK_UNDERFLOW error. Aborted.");
		exit(STACK_UNDERFLOW);
	}
	FloatStack *previousRoot = root;
	if (prevRootValue != NULL) {
		*prevRootValue = previousRoot->data;
	}
	
	root = root->next;
	
	free(previousRoot);
	previousRoot = NULL;
	
	return root;
}

float getTopFloat(const FloatStack *root) {
	return root->data;
}

void clearFloatStack(FloatStack **current) {
	FloatStack *prev = NULL;
	
	while (*current != NULL) {
		prev = *current;
		
		*current = (*current)->next;
		
		free(prev);
		prev = NULL;
	}
}

bool floatStackIsEmpty(FloatStack *root) {
	return root == NULL;
}