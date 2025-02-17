/*
    Лабораторная работа 4. Обработка одномерных массивов.
    Вариант 5, задание 4.4.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

typedef struct
{
	int* data;
	int size;
} DynamicArray;

void terminate(const char* message)
{
	puts(message);
	exit(EXIT_SUCCESS);
}

void swap(int *a, int *b) {
	int temp = *a;
	*a = *b;
	*b = temp;
}

bool found(int* x, size_t size, int element)
{
	for (size_t i = 0; i < size; i++)
	{
		if (x[i] == element)
		{
			return true;
		}
	}
	return false;
}

void removeDuplicate(DynamicArray* x) {
	int newSize = x->size;
	
	for (size_t i = 0; i < newSize; i++) {
		if (found(x->data, i, x->data[i])) {
			swap(&x->data[i], &x->data[newSize - 1]);
			newSize--;
			i--;
		}
	}
	
	x->data = realloc(x->data, newSize * sizeof(int));
	x->size = newSize;
}

int main()
{
	setlocale(LC_ALL, "rus");

	DynamicArray x;
	puts("Введите размер массива x: ");
	if (!scanf("%d", &x.size))
	{
		terminate("Проверьте корректность введённых данных!");
	}

	x.data = (int*)malloc(x.size * sizeof(int));
	puts("Введите элементы массива: ");
	for (size_t i = 0; i < x.size; i++) {
		if (!scanf("%d", &x.data[i])) {
			free(x.data);
			x.data = NULL;

			terminate("Проверьте корректность введённых данных!");
		}
	}
	
	removeDuplicate(&x);
	
	puts("Массив после удаления всех повторяющихся элементов: ");
	for (size_t i = 0; i < x.size; i++)
	{
		printf("%d ", x.data[i]);
	}
	
	free(x.data);
	x.data = NULL;

	return EXIT_SUCCESS;
}