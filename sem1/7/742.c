#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>

#define STRING_BUFFER_MAX_SIZE (1 << 10)

typedef struct Student
{
	char surname[STRING_BUFFER_MAX_SIZE];
	int group;
	int physicsGrade;
	int mathsGrade;
	int informaticsGrade;
	double GPA;
} Student;

typedef struct
{
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
	strcpy(students->data[students->size - 1].surname, s.surname);
	students->data[students->size - 1].group = s.group;
	students->data[students->size - 1].physicsGrade = s.physicsGrade;
	students->data[students->size - 1].mathsGrade = s.mathsGrade;
	students->data[students->size - 1].informaticsGrade = s.informaticsGrade;
	students->data[students->size - 1].GPA = s.GPA;
}

void removeLastStudent(StudentArray *students)
{
	if (students->size != 0)
	{
		students->data = (Student *)realloc(students->data,
		                                    --students->size * sizeof(Student));
	}
}

StudentArray getStudents(FILE *notes)
{
	StudentArray students;
	students.size = 0;
	students.data = NULL;
	
	Student currentStudent;
	currentStudent.group = 0;
	currentStudent.physicsGrade = 0;
	currentStudent.mathsGrade = 0;
	currentStudent.informaticsGrade = 0;
	currentStudent.GPA = .0f;
	
	char current;
	int currentNote = 1;
	char currentStr[STRING_BUFFER_MAX_SIZE];
	size_t currentStrIndex = 0;
	while ((current = (char)fgetc(notes)) != EOF)
	{
		if (current == '\n')
		{
			currentStr[currentStrIndex] = '\0';
			currentStrIndex = 0;
			
			switch (currentNote)
			{
				case 1:
					strcpy(currentStudent.surname, currentStr);
					break;
				case 2:
					currentStudent.group = atoi(currentStr);
					break;
				case 3:
					currentStudent.physicsGrade = atoi(currentStr);
					break;
				case 4:
					currentStudent.mathsGrade = atoi(currentStr);
					break;
				case 5:
					currentStudent.informaticsGrade = atoi(currentStr);
					break;
				case 6:
					currentStudent.GPA = atof(currentStr);
					break;
				default:
					addStudent(&students, currentStudent);
					
					currentNote = 0;
					break;
			}
			currentNote++;
		}
		else
		{
			currentStr[currentStrIndex++] = current;
		}
	}
	
	return students;
}

void writeStudentsToFile(FILE *notes, StudentArray students)
{
	for (size_t i = 0; i < students.size; i++)
	{
		fprintf(notes, "%s\n%d\n%d\n%d\n%d\n%lf\n\n",
		        students.data[i].surname, students.data[i].group,
		        students.data[i].physicsGrade, students.data[i].mathsGrade,
		        students.data[i].informaticsGrade, students.data[i].GPA);
	}
}

void readFile(FILE *file)
{
	char current;
	while ((current = (char)fgetc(file)) != EOF)
	{
		printf("%c", current);
	}
}

void swapStudents(Student *s1, Student *s2)
{
	if (s1 != s2)
	{
		Student tmp;
		strcpy(tmp.surname, s1->surname);
		tmp.group = s1->group;
		tmp.physicsGrade = s1->physicsGrade;
		tmp.mathsGrade = s1->mathsGrade;
		tmp.informaticsGrade = s1->informaticsGrade;
		tmp.GPA = s1->GPA;
		
		strcpy(s1->surname, s2->surname);
		s1->group = s2->group;
		s1->physicsGrade = s2->physicsGrade;
		s1->mathsGrade = s2->mathsGrade;
		s1->informaticsGrade = s2->informaticsGrade;
		s1->GPA = s2->GPA;
		
		strcpy(s2->surname, tmp.surname);
		s2->group = tmp.group;
		s2->physicsGrade = tmp.physicsGrade;
		s2->mathsGrade = tmp.mathsGrade;
		s2->informaticsGrade = tmp.informaticsGrade;
		s2->GPA = tmp.GPA;
	}
}

void createFile(const char *fileName)
{
	FILE *f = fopen(fileName, "w");
	fclose(f);
	puts("Файл создан.\n");
}

void viewFile(StudentArray students)
{
	for (size_t i = 0; i < students.size; i++)
	{
		printf("Фамилия: %s\n", students.data[i].surname);
		printf("Номер группы: %d\n", students.data[i].group);
		printf("Оценка за семестр по физике: %d\n",
		       students.data[i].physicsGrade);
		printf("Оценка за семестр по математике: %d\n",
		       students.data[i].mathsGrade);
		printf("Оценка за семестр по информатике: %d\n",
		       students.data[i].informaticsGrade);
		printf("Средний балл студента: %lf\n\n",
		       students.data[i].GPA);
	}
}

void addNote(FILE *notes)
{
	puts("Введите фамилию студента:");
	char surname[STRING_BUFFER_MAX_SIZE];
	scanf("%s", surname);
	
	puts("Введите номер группы студента:");
	char group[STRING_BUFFER_MAX_SIZE];
	scanf("%s", group);
	
	puts("Введите оценку за семестр студента по физике:");
	char physicsGrade[STRING_BUFFER_MAX_SIZE];
	scanf("%s", physicsGrade);
	
	puts("Введите оценку за семестр студента по математике:");
	char mathsGrade[STRING_BUFFER_MAX_SIZE];
	scanf("%s", mathsGrade);
	
	puts("Введите оценку за семестр студента по информатике:");
	char informaticsGrade[STRING_BUFFER_MAX_SIZE];
	scanf("%s", informaticsGrade);
	
	puts("Введите средний балл студента:");
	char GPA[STRING_BUFFER_MAX_SIZE];
	scanf("%s", GPA);
	
	fprintf(notes, "%s\n%s\n%s\n%s\n%s\n%s\n\n", surname, group,
	        physicsGrade, mathsGrade, informaticsGrade, GPA);
	
	puts("Запись добавлена.\n");
}

void solveIndividualTask(FILE *output, StudentArray students)
{
	for (size_t i = 0; i < students.size; i++)
	{
		if ((students.data[i].physicsGrade == 4 ||
		     students.data[i].physicsGrade == 5) &&
		    (students.data[i].mathsGrade > 8 &&
		     students.data[i].informaticsGrade > 8))
		{
			fprintf(output, "Фамилия: %s\n", students.data[i].surname);
			fprintf(output, "Номер группы: %d\n", students.data[i].group);
			fprintf(output, "Оценка за семестр по физике: %d\n",
			        students.data[i].physicsGrade);
			fprintf(output, "Оценка за семестр по математике: %d\n",
			        students.data[i].mathsGrade);
			fprintf(output, "Оценка за семестр по информатике: %d\n",
			        students.data[i].informaticsGrade);
			fprintf(output, "Средний балл студента: %lf\n\n",
			        students.data[i].GPA);
		}
	}
	
	puts("Решение индивидуального задания записано в файл.\n");
}

void editNote(StudentArray *students)
{
	puts("Введите фамилию студента, информацию о котором необходимо "
		 "редактировать:");
	char changingStudentsSurname[STRING_BUFFER_MAX_SIZE];
	scanf("%s", changingStudentsSurname);
	
	for (size_t i = 0; i < students->size; i++)
	{
		if (strcmp(students->data[i].surname, changingStudentsSurname) == 0)
		{
			int option = 1;
			while (1 <= option && option <= 6)
			{
				puts("Выберите информацию о студенте, которую хотите "
				     "редактиорвать:\n"
				     "1. Фамилия.\n"
				     "2. Номер группы.\n"
				     "3. Оценка за семестр по физике.\n"
				     "4. Оценка за семетр по математике.\n"
				     "5. Оценка за семестр по информатике.\n"
				     "6. Средний балл студента.\n"
					 "Любое другое число - выход из режима изменения");
				
				scanf("%d", &option);
				
				switch (option)
				{
					case 1:
						puts("Введите новую фамилию студента:");
						char surname[STRING_BUFFER_MAX_SIZE];
						scanf("%s", surname);
						strcpy(students->data[i].surname, surname);
						break;
					case 2:
						puts("Введите новый номер группы студента:");
						int group;
						scanf("%d", &group);
						students->data[i].group = group;
						break;
					case 3:
						puts("Введите новую оценку за семемтр по физике "
							 "студента:");
						int physicsGrade;
						scanf("%d", &physicsGrade);
						students->data[i].physicsGrade = physicsGrade;
						break;
					case 4:
						puts("Введите новую оценку за семемтр по математике "
						     "студента:");
						int mathsGrade;
						scanf("%d", &mathsGrade);
						students->data[i].physicsGrade = mathsGrade;
						break;
					case 5:
						puts("Введите новую оценку за семемтр по информатике "
						     "студента:");
						int informaticsGrade;
						scanf("%d", &informaticsGrade);
						students->data[i].informaticsGrade = informaticsGrade;
						break;
					case 6:
						puts("Введите новый средний балл студента:");
						double GPA;
						scanf("%lf", &GPA);
						students->data[i].GPA = GPA;
						break;
					default:
						puts("Выход из режимы изменения...");
				}
			}
			puts("Запись изменена.\n");
			return;
		}
	}
	puts("Нет такого студента!\n");
}

void removeNote(StudentArray *students)
{
	puts("Введите фамилию студента, информацию о котором необходимо "
	     "удалить (после удаления информации порядок следования студентов "
		 "в файле может быть нарушен):");
	char removingStudentsSurname[STRING_BUFFER_MAX_SIZE];
	scanf("%s", removingStudentsSurname);
	
	for (size_t i = 0; i < students->size; i++)
	{
		if (strcmp(students->data[i].surname, removingStudentsSurname) == 0)
		{
			swapStudents(&students->data[i],
						 &students->data[students->size - 1]);
			removeLastStudent(students);
			puts("Запись удалена.\n");
			return;
		}
	}
	puts("Нет такого студента!\n");
}

bool isSorted(Student s1, Student s2, int criterion)
{
	switch (criterion)
	{
		case 1:
			return strcmp(s1.surname, s2.surname) < 0;
		case 2:
			return s1.physicsGrade < s2.physicsGrade;
		case 3:
			return s1.physicsGrade > s2.physicsGrade;
		case 4:
			return s1.mathsGrade < s2.mathsGrade;
		case 5:
			return s1.mathsGrade > s2.mathsGrade;
		case 6:
			return s1.informaticsGrade < s2.informaticsGrade;
		case 7:
			return s1.informaticsGrade > s2.informaticsGrade;
		case 8:
			return s1.GPA < s2.GPA;
		case 9:
			return s1.GPA > s2.GPA;
		default:
			return false; // never get there
	}
}

void sortNotes(StudentArray *students)
{
	puts("Выберите критерий, по которому будет проводиться сортировка:\n"
		 "1. По алфавиту (фамилия).\n"
		 "2. По возрастанию оценки по физике.\n"
		 "3. По убыванию оценки по физике.\n"
		 "4. По возрастанию оценки по математике.\n"
	     "5. По убыванию оценки по математике.\n"
		 "6. По возрастанию оценки по информатике.\n"
		 "7. По убыванию оценки по информатике.\n"
	     "8. По возрастанию среднего балла.\n"
	     "9. По убыванию среднего балла.\n"
		 "Любое другое число - сортировка не производится.");
	int option;
	scanf("%d", &option);
	
	if (1 <= option && option <= 9)
	{
		for (size_t i = 0; i < students->size; i++)
		{
			for (size_t j = i + 1; j < students->size; j++)
			{
				if (!isSorted(students->data[i],
							  students->data[j], option))
				{
					swapStudents(&students->data[i], &students->data[j]);
				}
			}
		}
		puts("Сортировка выполнена.\n");
		return;
	}
	puts("Выход из режима сортировки...\n");
}

int main()
{
	setlocale(LC_ALL, "rus");
	
	int option = 1;
	while (1 <= option && option <= 8)
	{
		puts("Выберите операцию, которую хотите произвести:\n"
			 "1. Создание (создать файл записей).\n"
			 "2. Просмотр (просмотреть файл записей).\n"
			 "3. Просмотр (простое чтение файла).\n"
			 "4. Добавление (добавить запись в файл).\n"
			 "5. Решение индивидуального задания.\n"
			 "6. Редактировать (редактирование записи в файле).\n"
			 "7. Удаление (удалить запись из файла).\n"
			 "8. Сортировка (отсортировать записи в файле по критерию).\n"
			 "Любое другое число - выход из программы.");
		scanf("%d", &option);
		
		StudentArray students;
		students.size = 0;
		students.data = NULL;
		
		char fileName[STRING_BUFFER_MAX_SIZE];
		FILE *notes;
		
		char outputFileName[STRING_BUFFER_MAX_SIZE];
		FILE *output;
		
		switch (option)
		{
			case 1:
				puts("Введите название нового файла записей (если файл "
					 "записей существовал до этого, то все данные из него "
					 "будут удалены):");
				scanf("%s", fileName);
				
				createFile(fileName);
				
				break;
			case 2:
				puts("Введите название файла, содержимое которого вы хотите "
					 "просмотреть:");
				scanf("%s", fileName);
				
				notes = fopen(fileName, "r");
				students = getStudents(notes);
				fclose(notes);
				
				viewFile(students);
				
				break;
			case 3:
				puts("Введите название файла, содержимое которого вы хотите "
				     "просмотреть:");
				scanf("%s", fileName);
				
				FILE *file = fopen(fileName, "r");
				readFile(file);
				fclose(file);
				
				break;
			case 4:
				puts("Введите название файла, куда вы хотите добавить запись:");
				scanf("%s", fileName);
				
				notes = fopen(fileName, "a");
				addNote(notes);
				fclose(notes);
				
				break;
			case 5:
				puts("Введите название файла, информацию из которого вы "
					 "хотите получить:");
				scanf("%s", fileName);
				
				puts("Введите название файла, в который вы хотите записать "
				     "полученную информацию (если файла не существует, он "
					 "будет создан, а если существует, то вся находящаяся "
					 "в нем информация будет уничтожена):");
				scanf("%s", outputFileName);
				
				notes = fopen(fileName, "r");
				students = getStudents(notes);
				fclose(notes);
				
				output = fopen(outputFileName, "w");
				solveIndividualTask(output, students);
				fclose(output);
				
				break;
			case 6:
				puts("Введите название файла, информацию в котором вы "
				     "хотите изменить:");
				scanf("%s", fileName);
				
				notes = fopen(fileName, "r");
				students = getStudents(notes);
				fclose(notes);
				
				editNote(&students);
				
				notes = fopen(fileName, "w");
				writeStudentsToFile(notes, students);
				fclose(notes);
				
				break;
			case 7:
				puts("Введите название файла, информацию из которого вы "
				     "хотите удалить:");
				scanf("%s", fileName);
				
				notes = fopen(fileName, "r");
				students = getStudents(notes);
				fclose(notes);
				
				removeNote(&students);
				
				notes = fopen(fileName, "w");
				writeStudentsToFile(notes, students);
				fclose(notes);
				
				break;
			case 8:
				puts("Введите название файла, записи в котором вы "
				     "хотите отсортировать:");
				scanf("%s", fileName);
				
				notes = fopen(fileName, "r");
				students = getStudents(notes);
				fclose(notes);
				
				sortNotes(&students);
				
				notes = fopen(fileName, "w");
				writeStudentsToFile(notes, students);
				fclose(notes);
				
				break;
			default:
				puts("Выход из программы...");
		}
	}
	
	return EXIT_SUCCESS;
}