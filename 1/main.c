/*
    Лабораторная работа 1. Линейный вычислительный процесс.
    Вариант 5.
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

void input(const char* name, double* var)
{
    printf("%s %s%s", "Введите значение переменной", name, ": ");
    scanf("%lf", &(*var));
}

int main()
{
    setlocale(LC_ALL, "rus");

    // Ввод значений переменных x, y и z.
    double x, y, z;
    input("x", &x);
    input("y", &y);
    input("z", &z);

    // Вычисление первой части выражения
    double negSqrtAbsX = -sqrt(fabs(x));
    double part1 = log(pow(y, negSqrtAbsX));

    // Вычисление второй части выражения
    double part2 = x - y / 2;

    // Вычисление третьей части выражения
    double arctgZ = atan(z);
    double part3 = pow(sin(arctgZ), 2);

    // Получение результата
    double alpha = part1 * part2 + part3;

    printf("%s %lf",
           "Результат вычисления выражения из условия:",
           alpha);

    return EXIT_SUCCESS;
}