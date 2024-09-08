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

double input(const char* name)
{
    printf("Введите значение переменной %s: ", name);
    char varStr[32];
    fgets(varStr, 32, stdin);

    double var = atof(varStr);
    if (var == 0.0 && varStr[0] != '0')
    {
        terminate("Ошибка ввода! Неверно введено число.");
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

    double x = input("x");
    double y = input("y");
    double z = input("z");
    
    double numerator = min(z, x) + min(x, y);
    double denominator = pow(max3(x, y, z), 2);

    if (denominator == 0)
    {
        terminate("Ошибка! Деление на ноль.");
    }
    double m = numerator / denominator;

    printf("Результат вычисления значения выражения из условия: m=%lf\n", m);

    return EXIT_SUCCESS;
}