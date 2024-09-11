/*
    Лабораторная работа 3. Реализация циклических алгоритмов
    Вариант 5, задание 3.3.2
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <math.h>

void input(const char* name, const char* format, void* var)
{
    printf("Введите значение переменной %s: ", name);
    scanf(format, var);
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

double S(double x, int n)
{
    double result = 0;

    for (int k = 0; k < n; k++)
    {
        double numerator = cos(k * x);
        double denominator = factorial(k);
        
        result += (numerator / denominator);
    }

    return result;
}

double Y(double x)
{
    double factor1 = exp(cos(x));
    double factor2 = cos(sin(x));

    return factor1 * factor2;
}

int main()
{
    setlocale(LC_ALL, "rus");
    
    double a, b, h;
    int n;
    input("a", "%lf", &a);
    input("b", "%lf", &b);
    input("h", "%lf", &h);
    input("n", "%d", &n);

    for (double x = a; x <= b; x += h)
    {
        double Yx = Y(x);
        double Sx = S(x, n);
        double difference = fabs(Yx - Sx);

        printf("Y(x)=%lf\t", Yx);
        printf("S(x)=%lf\t", Sx);
        printf("|Y(x)-S(x)|=%lf\n", difference);
    }

    return EXIT_SUCCESS;
}

/*
    Для всех x изменяющихся от a=0.1 до b=1.0 с шагом h=0.1

    |Y(x)-S(x)|<0.001 при n=7
    |Y(x)-S(x)|<0.0001 при n=8
    |Y(x)-S(x)|<0.00001 при n=9
*/