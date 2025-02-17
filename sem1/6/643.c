/*
    Лабораторная работа 6. Использование строк
    Вариант 5, задание 6.4.3
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	char* data;
	int length;
} String;

typedef struct
{
	String* data;
	int size;
} StringArray;

void deleteWords(StringArray* words)
{
	for (int i = 0; i < words->size; i++)
	{
		words->data[i].length = 0;
		free(words->data[i].data);
		words->data[i].data = NULL;
	}
	words->size = 0;
	free(words->data);
	words->data = NULL;
}

void addCharacter(String* s, char c)
{
	s->data = (char*)realloc(s->data, ++s->length * sizeof(char));
	s->data[s->length - 1] = c;
}

void addElement(StringArray* arr, String s)
{
	arr->data = (String*)realloc(arr->data, ++arr->size * sizeof(String));
	arr->data[arr->size - 1] = s;
}

String getString()
{
	String str;
	str.length = 0;
	
	char currentCharacter = getchar();
	while (currentCharacter != '\n')
	{
		if (str.length == 0)
		{
			str.data = (char*)malloc((++str.length) * sizeof(char));
			str.data[str.length - 1] = currentCharacter;
		}
		else
		{
			addCharacter(&str, currentCharacter);
		}
		currentCharacter = getchar();
	}
	
	addCharacter(&str, '\0');
	
	return str;
}

StringArray getAllWords(String s)
{
	StringArray result;
	result.size = 1;
	result.data = (String*)malloc(result.size * sizeof(String));
	
	for (int i = 0; i < s.length; i++)
	{
		if (s.data[i] != ' ')
		{
			if (result.data[result.size - 1].length == 0)
			{
				result.data[result.size - 1].data =
						(char*)malloc((++result.data[result.size - 1].length) *
						sizeof(char));
				result.data[result.size - 1].data[0] = s.data[i];
			}
			else
			{
				addCharacter(&result.data[result.size - 1], s.data[i]);
			}
		}
		else
		{
			addCharacter(&result.data[result.size - 1], '\0');
			String newStr;
			newStr.length = 0;
			addElement(&result, newStr);
		}
	}
	addCharacter(&result.data[result.size - 1], '\0');
	
	return result;
}

int main()
{
	printf("Введите строку: ");
	String str = getString();
	
	StringArray words = getAllWords(str);
	
	printf("Количество слов в тексте: %d, "
	       "список слов:\n", words.size);
	for (int i = 0; i < words.size; i++)
	{
		printf("%d. \"%s\"", i + 1, words.data[i].data);
		if (i + 1 < words.size)
		{
			printf("\n");
		}
	}
	
	deleteWords(&words);
	str.length = 0;
	free(str.data);
	str.data = NULL;
	
	return EXIT_SUCCESS;
}