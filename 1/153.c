/*
    Лабораторная работа 1. Линейный вычислительный процесс
    Вариант 5, задание 1.5.3
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

int main()
{
    setlocale(LC_ALL, "rus");

    double x = input("Введите значение переменной x (вещественное число): ");
    double y = input("Введите значение переменной y (вещественное число): ");
    double z = input("Введите значение переменной z (вещественное число): ");

    double negSqrtAbsX = -sqrt(fabs(x));
    double part1 = log(pow(y, negSqrtAbsX));

    double part2 = x - y / 2;
    
    double arctgZ = atan(z);
    double part3 = pow(sin(arctgZ), 2);

    double alpha = part1 * part2 + part3;

    printf("Результат вычисления выражения из условия: %lf\n", alpha);

    return EXIT_SUCCESS;
}