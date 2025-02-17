/*
    Лабораторная работа 2. Реализация разветвляющихся алгоритмов
    Вариант 5, задание 2.4.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

void terminate(const char* message)
{
    puts(message);
    exit(EXIT_SUCCESS);
}

double input(const char* message)
{
    puts(message);

    double var;
    if (!scanf("%lf", &var))
    {
        terminate("Проверьте корректность введённых данных!");
    }

    return var;
}

double max(double a, double b)
{
    if (a > b)
    {
        return a;
    }
    return b;
}

double min(double a, double b)
{
    if (a < b)
    {
        return a;
    }
    return b;
}

double max3(double a, double b, double c)
{
    return max(a, max(b, c));
}

int main()
{
    setlocale(LC_ALL, "rus");

    double x = input("Введите значение переменной x (вещественное число): ");
    double y = input("Введите значение переменной y (вещественное число): ");
    double z = input("Введите значение переменной z (вещественное число): ");

    double numerator = min(z, x) + min(x, y);
    double denominator = pow(max3(x, y, z), 2);

    if (denominator == 0)
    {
        terminate("Ошибка! Деление на ноль.");
    }
    double m = numerator / denominator;

    printf("Результат вычисления значения выражения из условия: m = %lf\n", m);

    return EXIT_SUCCESS;
}