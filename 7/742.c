/*
    Лабораторная работа 7. Обработка структур с использованием файлов
    Вариант 5, задание 7.4.2
*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

typedef struct
{
	char *data;
	int length;
} String;

void addCharacter(String *s, char c)
{
	if (s->length == 0)
	{
		s->data = (char *)malloc(++s->length * sizeof(char));
	}
	else
	{
		s->data = (char *)realloc(s->data, ++s->length * sizeof(char));
	}
	s->data[s->length - 1] = c;
	if (c == '\0')
	{
		s->length--;
	}
}

void clearString(String* s)
{
	free(s->data);
	s->data = NULL;
	s->length = 0;
}

String getString()
{
	String str;
	str.length = 0;
	
	char currentCharacter = getchar();
	while (currentCharacter != '\n' && currentCharacter != EOF)
	{
		addCharacter(&str, currentCharacter);
		currentCharacter = getchar();
	}
	
	addCharacter(&str, (char)((currentCharacter == EOF) ? EOF : '\0'));
	
	return str;
}

typedef struct Student
{
	String surname;
	int group;
	int physicsGrade;
	int mathsGrade;
	int informaticsGrade;
	float GPA;
} Student;

typedef struct {
	Student *data;
	size_t size;
} StudentArray;

void addStudent(StudentArray *students, Student s)
{
	if (students->size == 0)
	{
		students->data = (Student *)malloc(++students->size * sizeof(Student));
	}
	else
	{
		students->data = (Student *)realloc(students->data,
								 ++students->size * sizeof(Student));
	}
	for (size_t i = 0; i < s.surname.length; i++)
	{
		addCharacter(&students->data[students->size - 1].surname,
					 s.surname.data[i]);
	}
	students->data[students->size - 1].group = s.group;
	students->data[students->size - 1].physicsGrade = s.physicsGrade;
	students->data[students->size - 1].mathsGrade = s.mathsGrade;
	students->data[students->size - 1].informaticsGrade = s.informaticsGrade;
	students->data[students->size - 1].GPA = s.GPA;
}

int main()
{
	setlocale(LC_ALL, "rus");
	
	StudentArray students;
	students.size = 0;
	students.data = NULL;
	
	puts("Введите имя файла записей:");
	String noteFileName = getString();
	
	puts("Введите имя выходного файла:");
	String outputFileName = getString();
	
	freopen(noteFileName.data, "r", stdin);
	
	String currentString = getString();
	Student currentStudent;
	currentStudent.surname.length = 0;
	currentStudent.surname.data = NULL;
	currentStudent.group = 0;
	currentStudent.physicsGrade = 0;
	currentStudent.mathsGrade = 0;
	currentStudent.informaticsGrade = 0;
	currentStudent.GPA = .0f;
	int currentNote = 1;
	while (currentString.data[currentString.length - 1] != EOF)
	{
		switch (currentNote)
		{
			case 1: // фамилия
				for (size_t i = 0; i < currentString.length; i++)
				{
					addCharacter(&currentStudent.surname,
								 currentString.data[i]);
				}
				break;
			case 2: // номер группы
				currentStudent.group = atoi(currentString.data);
				break;
			case 3: // оценка по физике
				currentStudent.physicsGrade = atoi(currentString.data);
				break;
			case 4: // оценка по математике
				currentStudent.mathsGrade = atoi(currentString.data);
				break;
			case 5: // оценка по информатике
				currentStudent.informaticsGrade = atoi(currentString.data);
				break;
			case 6: // средний балл
				currentStudent.GPA = atof(currentString.data);
				break;
		}
		
		currentNote++;
		if (currentNote > 6)
		{
			addStudent(&students, currentStudent);
			clearString(&currentStudent.surname);
			currentStudent.group = 0;
			currentStudent.physicsGrade = 0;
			currentStudent.mathsGrade = 0;
			currentStudent.informaticsGrade = 0;
			currentStudent.GPA = .0f;
			
			currentNote = 1;
			currentString = getString(); // пустая строчка после очередной записи
		}
		
		clearString(&currentString);
		currentString = getString();
	}
	
	fclose(stdin);
	
	freopen(outputFileName.data, "w", stdout);
	
	for (size_t i = 0; i < students.size; i++)
	{
		if ((students.data[i].physicsGrade == 4 ||
			students.data[i].physicsGrade == 5) &&
			(students.data[i].mathsGrade > 8 &&
			students.data[i].informaticsGrade > 8))
		{
			printf("Фамилия: %s\nНомер группы: %d\nОценка за семестр "
				   "по физике: %d\nОценка за семестр по математике: %d\n"
				   "Оценка за семестр по информатике: %d\nСредний балл: %f\n\n",
				   students.data[i].surname.data, students.data[i].group,
				   students.data[i].physicsGrade, students.data[i].mathsGrade,
				   students.data[i].informaticsGrade, students.data[i].GPA);
		}
	}
	
	fclose(stdout);
	
	return EXIT_SUCCESS;
}