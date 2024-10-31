/*
    Лабораторная работа 8. Функции пользователя
    Вариант 5, задание 8.3.3
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <time.h>

#define BUFFER_LENGTH (1 << 10)

size_t length(const char *s)
{
	size_t len = 0;
	while (s[++len] != '\0')
	{}
	return len;
}

size_t _recursiveLength(const char *s, size_t curLen)
{
	if (s[curLen] == '\0')
	{
		return curLen;
	}
	return _recursiveLength(s, curLen + 1);
}

size_t recursiveLength(const char *s)
{
	return _recursiveLength(s, 0);
}

float timeForLength(const char *s)
{
	clock_t start = clock();
	size_t len = length(s);
	clock_t finish = clock();
	return ( (float)finish - (float)start ) / CLOCKS_PER_SEC;
}

float timeForRecursiveLength(const char *s)
{
	clock_t start = clock();
	size_t len = recursiveLength(s);
	clock_t finish = clock();
	return ( (float)finish - (float)start ) / CLOCKS_PER_SEC;
}

int main()
{
	setlocale(LC_ALL, "rus");
	
	puts("Введите строку:");
	char str[BUFFER_LENGTH];
	scanf("%s", str);
	
	clock_t start1 = clock();
	size_t len = length(str);
	clock_t finish1 = clock();
	float standardTime = ( (float)finish1 - (float)start1 ) / CLOCKS_PER_SEC;
	
	clock_t start2 = clock();
	size_t lenRecursive = recursiveLength(str);
	clock_t finish2 = clock();
	float recursiveTime = ( (float)finish2 - (float)start2 ) / CLOCKS_PER_SEC;
	
	printf("Длина строки (посчитано стандартным методом) равна %zu\n"
		   "Выполнение данной функции заняло %f секунд\n\n",
		   len, standardTime);
	printf("Длина строки (посчитано рекурсивным методом) равна %zu\n"
		   "Выполнение данной функции заняло %f секунд\n\n",
		   lenRecursive, recursiveTime);
	
	
	return EXIT_SUCCESS;
}

/*
 * Тестовая строка:
 *
 * QKWEJHFIUQLWHXFEIUWFHWEIFHWUIFHWEIJFHWEUIFHWERUIHGOERUINFGYUERIWFYNWPEOUIYFPNYUWEPYUPENWIRYERIUYWCINUFYERNPUIREYRNIUWNIROWQYNPEUPIWRYWUPRQYWIRYWEQNIRYWCNPIRUQYWRNIPUYRNIWEQPURYCNWEIRPYUEWIPUQYWCNRIQWUYRCNQOIWUYRCNWIOQURYQWEINUCNOUWEYNIOCWQYRWIOURYCNQWIORYUWENORIYWNCWYRINOUEWYREWIONURYNEIUORAYEWUIONYRQIOUYWRENIUENRICWQERYNIWEORYUEIORYQWEOIURYWQOIURYCWNIRUWEYRIUOQWHRUIOQWYRIUQEWYRCUIEOROYQWNIURYQWIUEORYWIUOYRQNUWICRYNWOIURYNWORIEIQWEQUWRYCNWOIUQRUCNWOIUYECNWOIRQYCNQIURYWQNOUIRYQCIWUERYINQWOURYEWCNRIOQUWYRWQIUORYCNQWEIORUYCWEINORUYWINOUQRYWQIOURYWERUIQYEWRCIOQNRYNEIWORUYWEIORUNWCIROUYOEWONRQWYIUEONQCUYOWIERNIUWRYNEWIOURYCNOWIEURYCQNOIRYENWUIYNCQWIUOERYNCIWOUEYRCNQWOIERYOIEWRYOIQWEUNROIUQNROIQWUENRIOUWEYOQIWUEQOIERUYNEQWOIRUYCNQOIUEYRENWORQUYCWN
 */