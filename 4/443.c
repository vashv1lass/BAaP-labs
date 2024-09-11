/*
    Лабораторная работа 4. Обработка одномерных массивов.
    Вариант 5, задание 4.4.3

    Реализация "в лоб" O(n^2). Для ускорения программы до O(n*log(n)) можно
    решить задачу при помощи быстрой сортировки или сортировки слиянием.
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>

void terminate(const char* message)
{
    puts(message);
    exit(EXIT_SUCCESS);
}

bool found(int* x, int size, int element)
{
    for (int i = 0; i < size; i++)
    {
        if (x[i] == element)
        {
            return true;
        }
    }
    return false;
}

typedef struct
{
    int* data;
    int size;
} DynamicArray;


DynamicArray removeDuplicate(DynamicArray x)
{
    DynamicArray unique;
    unique.size = 1;
    unique.data = (int*)malloc(unique.size * sizeof(int));
    unique.data[0] = x.data[0];

    for (int i = 1; i < x.size; i++)
    {
        if (!found(unique.data, unique.size, x.data[i]))
        {
            unique.data = (int*)realloc(unique.data, ++unique.size * sizeof(int));
            unique.data[unique.size - 1] = x.data[i];
        }
    }

    return unique;
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
    for (int i = 0; i < x.size; i++)
    {
        if (!scanf("%d", &x.data[i]))
        {
            x.data = NULL;
            free(x.data);

            terminate("Проверьте корректность введённых данных!");
        }
    }

    DynamicArray unique = removeDuplicate(x);
    puts("Массив после удаления всех повторяющихся элементов: ");
    for (int i = 0; i < unique.size; i++)
    {
        printf("%d ", unique.data[i]);
    }
    puts("");

    x.data = NULL;
    free(x.data);
    unique.data = NULL;
    free(unique.data);

    return EXIT_SUCCESS;
}