// Лабораторная работа №3. Динамическая структура очередь.
// Вариант 8.

#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "algo.h"

int main(void) {
	queue q = new_queue();
	
	int option = 1;
	while (1 <= option && option <= 4) {
		puts("Введите число, соответствующее операции, которую вы хотите провести:\n"
			 "1 - добавление элемента в очередь.\n"
			 "2 - удаление элемента из очереди.\n"
			 "3 - просмотр очереди.\n"
			 "4 - решение индивидуального задания.\n"
			 "Любое другое число – выход из программы.");
		scanf("%d", &option);
		
		if (option == 1) {
			int push_option = 0;
			
			puts("Выберите, куда вы хотите добавить элемент:\n"
				 "1 - в начало.\n"
				 "2 - в конец.\n"
				 "Любое другое число - не хочу добавлять элемент.");
			scanf("%d", &push_option);
			if (push_option != 1 && push_option != 2) {
				continue;
			}
			
			int element;
			printf("Введите элемент, который хотите добавить в очередь: ");
			scanf("%d", &element);
			if (push_option == 1) {
				push_front(&q, element);
			} else if (push_option == 2) {
				push(&q, element);
			}
			puts("Элемент добавлен.");
		} else if (option == 2) {
			int pop_option = 0;
			
			puts("Выберите, откуда вы хотите удалить элемент:\n"
			     "1 - с начала.\n"
			     "2 - с конца.\n"
			     "Любое другое число - не хочу удалять элемент.");
			scanf("%d", &pop_option);
			if (pop_option != 1 && pop_option != 2) {
				continue;
			}
			
			if (empty(q)) {
				puts("Очередь пуста. удаление элемента невозможно!");
				continue;
			}
			
			int deleted_element = -1;
			if (pop_option == 1) {
				deleted_element = pop(&q);
			} else if (pop_option == 2) {
				deleted_element = pop_rear(&q);
			}
			printf("Удалённый элемент: %d\n", deleted_element);
		} else if (option == 3) {
			int view_option = 0;
			
			puts("Выберите, откуда вы хотите просмотреть очередь:\n"
			     "1 - с начала.\n"
			     "2 - с конца.\n"
			     "Любое другое число - не хочу просматривать очередь.");
			scanf("%d", &view_option);
			if (view_option != 1 && view_option != 2) {
				continue;
			}
			
			if (view_option == 1) {
				view_front(q);
			} else if (view_option == 2) {
				view_rear(q);
			}
		} else if (option == 4) {
			if (empty(q)) {
				puts("Решение индивидуального задания невозможно т. к. очередь пуста");
				continue;
			}
			
			queue_node *min = find_min(q);
			if (min == NULL) {
				puts("Решение индивидуального задания невозможно т. к. в очереди несколько минимумов");
				continue;
			}
			if (min == q.front) {
				puts("Первый элемент является минимальным.");
				continue;
			}
			if (q.front == prev_node(q, min)) {
				puts("Между первым элементом и минимальным ничего нет.");
				continue;
			}
			
			queue saved_q = copy_queue(q);
			
			queue_node *elements_between_first_and_min = q.front->next;
			queue_node *elements_before_min = prev_node(q, min);
			queue_node *elements_remaining = q.front;
			
			q.front->next = min;
			elements_before_min->next = NULL;
			
			puts("Элементы очереди, находящиеся между первым элементом очереди и ее минимальным элементом:");
			while (elements_between_first_and_min != NULL) {
				printf("%d ", elements_between_first_and_min->info);
				queue_node *prev = elements_between_first_and_min;
				elements_between_first_and_min = elements_between_first_and_min->next;
				free(prev);
			}
			
			puts("\nЭлементы очереди, не находящиеся на этих позициях:");
			while (elements_remaining != NULL) {
				printf("%d ", elements_remaining->info);
				queue_node *prev = elements_remaining;
				elements_remaining = elements_remaining->next;
				free(prev);
			}
			puts("");
			
			q = copy_queue(saved_q);
		}
	}
	
	delete_queue(&q);
	
	return 0;
}