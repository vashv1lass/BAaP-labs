// Лабораторная работа №2. Динамическая структура стек.
// Вариант 8.

#include <stdio.h>
#include <locale.h>
#include <limits.h>

#include "Stack.h"

void print(Stack **s) {
	Stack *auxS = NULL;
	while (!isEmpty(*s)) {
		int currentValue = 0;
		*s = pop(*s, &currentValue);
		
		auxS = push(auxS, currentValue);
		
		printf("%d ", currentValue);
	}
	puts("");
	while (!isEmpty(auxS)) {
		int currentAuxValue = 0;
		auxS = pop(auxS, &currentAuxValue);
		
		*s = push(*s, currentAuxValue);
	}
}

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

void reversedCopy(Stack **s, Stack **copy) {
	clear(&(*copy));
	
	Stack *auxS = NULL;
	while (!isEmpty(*s)) {
		int currentValue = 0;
		*s = pop(*s, &currentValue);
		
		auxS = push(auxS, currentValue);
		
		*copy = push(*copy, currentValue);
	}
	
	while (!isEmpty(auxS)) {
		int currentAuxValue = 0;
		auxS = pop(auxS, &currentAuxValue);
		
		*s = push(*s, currentAuxValue);
	}
}

int main(void) {
	setlocale(LC_ALL, "rus");
	
	Stack *sRoot = NULL;
	
	int option = 1;
	while (1 <= option && option <= 4) {
		puts("Выберите операцию, которую хотите выполнить:\n"
			 "1. Добавление элемента в стек.\n"
			 "2. Удаление верхнего элемента из стека.\n"
			 "3. Просмотр стека.\n"
			 "4. Решение индивидуального задания.\n"
			 "Любое другое число – выход из программы.");
		scanf("%d", &option);
		
		if (option == 1) {
			puts("Введите добавляемый элемент:");
			
			int element;
			scanf("%d", &element);
			sRoot = push(sRoot, element);
		} else if (option == 2) {
			int popped = 0;
			sRoot = pop(sRoot, &popped);
			
			printf("Удалённый элемент: %d\n", popped);
		} else if (option == 3) {
			printf("Стек: ");
			print(&sRoot);
		} else if (option == 4) {
			if (sRoot == NULL) {
				puts("Решение индивидуального задания невозможно. "
					 "Добавьте элементы в стек.");
			} else {
				Stack *auxS = NULL;
				reversedCopy(&sRoot, &auxS);
				
				Stack *elementsBetween1stAndMin = getNext(sRoot);
				Stack *minElement = findMin(&sRoot);
				Stack *elementBeforeMin = getElementBefore(&sRoot, minElement);
				Stack *elementsRemaining = sRoot;
				
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
				while (!isEmpty(elementsRemaining)) {
					int currentElementValue = 0;
					elementsRemaining = pop(elementsRemaining, &currentElementValue);
					
					printf("%d ", currentElementValue);
				}
				puts("");
				
				clear(&elementsBetween1stAndMin);
				clear(&elementsRemaining);
				
				sRoot = NULL;
				while (!isEmpty(auxS)) {
					int currentAuxValue = 0;
					auxS = pop(auxS, &currentAuxValue);
					
					sRoot = push(sRoot, currentAuxValue);
				}
			}
		}
	}
	
	return 0;
}