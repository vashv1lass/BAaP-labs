// Лабораторная работа №1. Рекурсивные функции
// Вариант 8.

#include <stdio.h>
#include <stdlib.h>

int maxElementRecursive(int *a, size_t lhs, size_t rhs) {
	if (lhs == rhs) {
		return a[lhs];
	}
	if (rhs - lhs + 1 == 2) {
		return ((a[lhs] > a[rhs]) ? a[lhs] : a[rhs]);
	}
	size_t mid = (lhs + rhs) / 2;
	int maxFromLeft = maxElementRecursive(a, lhs, mid);
	int maxFromRight = maxElementRecursive(a, mid, rhs);
	return ((maxFromLeft > maxFromRight) ? maxFromLeft : maxFromRight);
}

int maxElement(int *a, size_t sz) {
	return maxElementRecursive(a, 0, sz);
}

int main(void) {
	size_t n;
	scanf("%zu", &n);
	int *a = malloc(n * sizeof(int));
	for (size_t i = 0; i < n; i++) {
		scanf("%d", &a[i]);
	}
	printf("%d\n", maxElement(a, n));
	return 0;
}