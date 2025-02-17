// Лабораторная работа №2. Динамическая структура стек.
// Вариант 8.

#include <stdio.h>
#include <locale.h>
#include <limits.h>

#include "Stack.h"

Stack *findMin(Stack **s) {
	int minValue = INT_MAX;
	
	Stack *auxS = NULL;
	while (!isEmpty(*s)) {
		int currentValue = 0;
		*s = pop(*s, &currentValue);
		
		auxS = push(auxS, currentValue);
		
		if (currentValue < minValue) {
			minValue = currentValue;
		}
	}
	
	Stack *minElement = NULL;
	while (!isEmpty(auxS)) {
		int currentAuxSValue = 0;
		auxS = pop(auxS, &currentAuxSValue);
		
		*s = push(*s, currentAuxSValue);
		if (minValue == currentAuxSValue) {
			minElement = *s;
		}
	}
	
	return minElement;
}

Stack *getElementBefore(Stack **s, Stack *element) {
	Stack *auxS = NULL;
	
	while (!isEmpty(*s) && getNext(*s) != element) {
		int currentValue = 0;
		*s = pop(*s, &currentValue);
		
		auxS = push(auxS, currentValue);
	}
	bool wasEmpty = isEmpty(*s);
	
	Stack *elementBefore = *s;
	
	while (!isEmpty(auxS)) {
		int currentAuxSValue = 0;
		auxS = pop(auxS, &currentAuxSValue);
		
		*s = push(*s, currentAuxSValue);
	}
	
	if (wasEmpty) {
		return *s;
	}
	return elementBefore;
}

int main(void) {
	setlocale(LC_ALL, "rus");
	
	size_t n;
	
	printf("Введите кол-во элементов списка: ");
	scanf("%zu", &n);
	
	Stack *sRoot = NULL;
	puts("Введите элементы списка:");
	for (size_t i = 0; i < n; i++) {
		int currentElement;
		scanf("%d", &currentElement);
		
		sRoot = push(sRoot, currentElement);
	}
	
	Stack *elementsBetween1stAndMin = getNext(sRoot);
	Stack *minElement = findMin(&sRoot);
	Stack *elementBeforeMin = getElementBefore(&sRoot, minElement);
	Stack *elementRemaining = sRoot;
	
	if (minElement == sRoot || minElement == getNext(sRoot)) {
		elementsBetween1stAndMin = NULL;
	} else {
		setNext(sRoot, minElement);
		setNext(elementBeforeMin, NULL);
	}
	
	puts("Элементы стека, содержащего элементы между вершиной и минимальным элементом стека:");
	while (!isEmpty(elementsBetween1stAndMin)) {
		int currentElementValue = 0;
		elementsBetween1stAndMin = pop(elementsBetween1stAndMin, &currentElementValue);
		
		printf("%d ", currentElementValue);
	}
	
	puts("\nЭлементы стека, в котором нет таких элементов");
	while (!isEmpty(elementRemaining)) {
		int currentElementValue = 0;
		elementRemaining = pop(elementRemaining, &currentElementValue);
		
		printf("%d ", currentElementValue);
	}
	
	return 0;
}