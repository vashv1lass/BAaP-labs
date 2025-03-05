#include "Lab.h"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Tree.h"

void leavesCountOnEachLevelHelper(Node *currentNode,
		int *leavesCount, int currentLevel) {
	if (currentNode != NIL()) {
		if (currentNode->left == NIL() && currentNode->right == NIL()) {
			leavesCount[currentLevel]++;
		}
		
		leavesCountOnEachLevelHelper(currentNode->left,
		                             leavesCount, currentLevel + 1);
		leavesCountOnEachLevelHelper(currentNode->right,
		                             leavesCount, currentLevel + 1);
	}
}

void leavesCountOnEachLevel(Tree tree, int *leavesCount) {
	leavesCountOnEachLevelHelper(tree.root, leavesCount, 0);
}

int main(void) {
        setlocale(LC_ALL, "rus");

	Tree tree;
	initialize(&tree);

	int option = 1;
	while (1 <= option && option <= 5) {
		puts("Выберите операцию:\n"
		     "1. Добавить в дерево поиска новую запись.\n"
		     "2. Удалить из дерева поиска информацию с заданным ключом.\n"
		     "3. Найти информацию в дереве поиска по ключу.\n"
		     "4. Вывести дерево поиска на экран.\n"
		     "5. Решить индивидуальное задание.\n"
                     "Любое другое число – выход из программы.");
		scanf("%d", &option);

		if (option == 1) {
			int key;
			printf("Введите ключ записи: ");
			scanf("%d", &key);

			char value[STRING_BUFFER_MAX_SIZE];
			printf("Введите значение записи: ");
			scanf("%s", value);

			insert(&tree, key, value);
		} else if (option == 2) {
			int key;
			printf("Введите ключ удаляемой записи: ");
			scanf("%d", &key);

			erase(&tree, key);
		} else if (option == 3) {
			int key;
			printf("Введите ключ, по которому вы хотите получить "
				   "информацию: ");
			scanf("%d", &key);

			const char *foundValue = find(tree, key);
			if (foundValue == NULL) {
				printf("Значение, соответствующее ключу %d не найдено!\n",
					   key);
			} else {
				printf("Значение: %s\n", foundValue);
			}
		} else if (option == 4) {
			printf("Дерево выглядит так:\n");
			print(tree);
		} else if (option == 5) {
			if (empty(tree)) {
				printf("Дерево пустое. Невозможно решить задачу!\n");
			} else {
				size_t levelCount = (size_t)treeLevelCount(tree);
				int leavesCount[levelCount];
				for (size_t i = 0; i < levelCount; i++) {
					leavesCount[i] = 0;
				}
				
				leavesCountOnEachLevel(tree, leavesCount);
				
				for (size_t i = 0; i < levelCount; i++) {
					printf("Количество листьев на %zu-м уровне дерева: %d\n",
					       i, leavesCount[i]);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}