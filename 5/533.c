/*
    Лабораторная работа 5. Обработка двумерных динамеческих массивов.
    Вариант 5, задание 5.3.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stdbool.h>
#include <string.h>

typedef struct
{
	int** data;
	int rowCount;
	int columnCount;
} Matrix;

typedef struct
{
	int row;
	int column;
} MatrixIndex;

typedef struct
{
	MatrixIndex* data;
	int size;
} MatrixIndexArray;

void terminate(const char* message)
{
	puts(message);
	exit(EXIT_SUCCESS);
}

int min(int a, int b)
{
	return ((a < b) ? a : b);
}

int max(int a, int b)
{
	return ((a > b) ? a : b);
}

void createMatrix(Matrix* a, int rows, int columns)
{
	a->rowCount = rows;
	a->columnCount = columns;
	a->data = (int**)malloc(rows * sizeof(int*));
	for (int i = 0; i < rows; i++)
	{
		a->data[i] = (int*)malloc(columns * sizeof(int));
	}
}

void deleteMatrix(Matrix* a)
{
	for (int i = 0; i < a->rowCount; i++)
	{
		free(a->data[i]);
		a->data[i] = NULL;
	}
	
	free(a->data);
	a->data = NULL;
	
	a->rowCount = 0;
	a->columnCount = 0;
}

int findMinInRow(Matrix a, int row)
{
	int minElement = a.data[row][0];
	for (int i = 1; i < a.columnCount; i++)
	{
		minElement = min(minElement, a.data[row][i]);
	}
	return minElement;
}

int findMaxInRow(Matrix a, int row)
{
	int maxElement = a.data[row][0];
	for (int i = 1; i < a.columnCount; i++)
	{
		maxElement = max(maxElement, a.data[row][i]);
	}
	return maxElement;
}

int findMinInColumn(Matrix a, int col)
{
	int minElement = a.data[0][col];
	for (int i = 1; i < a.rowCount; i++)
	{
		minElement = min(minElement, a.data[i][col]);
	}
	return minElement;
}

int findMaxInColumn(Matrix a, int col)
{
	int maxElement = a.data[0][col];
	for (int i = 1; i < a.rowCount; i++)
	{
		maxElement = max(maxElement, a.data[i][col]);
	}
	return maxElement;
}

bool isSpecial(Matrix a, int row, int col)
{
	int rowMin = findMinInRow(a, row);
	int rowMax = findMaxInRow(a, row);
	
	int colMin = findMinInColumn(a, col);
	int colMax = findMaxInColumn(a, col);
	
	return ((a.data[row][col] == rowMin && a.data[row][col] == colMax) ||
	        (a.data[row][col] == rowMax && a.data[row][col] == colMin));
}

MatrixIndexArray findAllSpecialElements(Matrix a)
{
	MatrixIndexArray result;
	result.size = 0;
	
	for (int i = 0; i < a.rowCount; i++)
	{
		for (int j = 0; j < a.columnCount; j++)
		{
			if (isSpecial(a, i, j))
			{
				if (result.size == 0)
				{
					result.data = (MatrixIndex*)malloc(++result.size * sizeof(MatrixIndex));
				}
				else
				{
					result.data = (MatrixIndex*)realloc(result.data, ++result.size * sizeof(MatrixIndex));
				}
				
				MatrixIndex currentElementIndex;
				currentElementIndex.row = i;
				currentElementIndex.column = j;
				
				result.data[result.size - 1] = currentElementIndex;
			}
		}
	}
	
	return result;
}

int main()
{
	setlocale(LC_ALL, "rus");
	
	printf("Введите количество строк матрицы n: ");
	int n, m;
	if (!scanf("%d", &n) || n <= 0)
	{
		terminate("Введите корректное значние размера!");
	}
	
	printf("Введите количество столбцов матрицы m: ");
	if (!scanf("%d", &m) || m <= 0)
	{
		terminate("Введите корректное значние размера!");
	}
	
	Matrix a;
	createMatrix(&a, n, m);
	
	puts("Введите матрицу размером n*m:");
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			scanf("%d", &a.data[i][j]);
		}
	}
	
	puts("Индексы всех \"особых\" элементов матрицы:");
	MatrixIndexArray answer = findAllSpecialElements(a);
	for (int i = 0; i < answer.size; i++)
	{
		printf("[%d, %d]", answer.data[i].row + 1, answer.data[i].column + 1);
		if (i + 1 != answer.size)
		{
			printf("; ");
		}
	}
	puts("");
	
	answer.size = 0;
	free(answer.data);
	answer.data = NULL;
	
	deleteMatrix(&a);
	
	return EXIT_SUCCESS;
}

/*
 * ТЕСТ ДЛЯ ПРОВЕРКИ РАБОТЫ ПРОГРАММЫ.
 * ВВОД:
 * n=3, m=3;
 * a = {
 *       { 1, 2, 3 },
 *       { 5, 6, 7 },
 *       { 3, 1, 9 }
 *     }
 * ВЫВОД: [1, 3]; [2, 1]
 */