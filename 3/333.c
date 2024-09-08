/*
    Лабораторная работа 3. Реализация циклических алгоритмов
    Вариант 5, задание 3.3.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

void input(const char* name, double* var)
{
    printf("Введите значение переменной %s: ", name);
    scanf("%lf", var);
}

int factorial(int n)
{
    int result = 1;

    for (int i = 2; i <= n; i++)
    {
        result *= i;
    }

    return result;
}

double Y(double x)
{
    double factor1 = exp(cos(x));
    double factor2 = cos(sin(x));

    return factor1 * factor2;
}

typedef struct
{
    double Sx;
    int numberOfSteps;
} SxReturnValue;

SxReturnValue S(double x, double eps)
{
    double sum = 0;
    int k = 0; // k - число шагов

    while (fabs(sum - Y(x)) >= eps)
    {
        double numerator = cos(k * x);
        double denominator = factorial(k);

        sum += (numerator / denominator);
        k++;
    }

    SxReturnValue result;
    result.Sx = sum;
    result.numberOfSteps = k;
    
    return result;
}

int main()
{
    double a, b, h, eps;
    input("a", &a);
    input("b", &b);
    input("h", &h);
    input("eps", &eps);

    for (double x = a; x <= b; x += h)
    {
        SxReturnValue Sx = S(x, eps);
        double Yx = Y(x);

        printf("S(x)=%lf\t", Sx.Sx);
        printf("Y(x)=%lf\t", Yx);
        printf("Количество шагов чтобы сделать |Y(x)-S(x)|<%lf равно %d\n",
               eps, Sx.numberOfSteps);
    }

    return EXIT_SUCCESS;
}