/*
    Лабораторная работа 1. Линейный вычислительный процесс.
    Вариант 5.
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

int main()
{
    setlocale(LC_ALL, "rus");
    
    double x = input("x");
    double y = input("y");
    double z = input("z");

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

    printf("Результат вычисления выражения из условия: %lf\n", alpha);

    return EXIT_SUCCESS;
}