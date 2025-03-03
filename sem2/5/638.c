#include <stdio.h>
#include <stdlib.h>

#include "Tree.h"

int main(void) {
	Tree tree;
	initialize(&tree);
	
	insert(&tree, 24, "x");
	insert(&tree, 5, "e");
	insert(&tree, 1, "a");
	insert(&tree, 15, "o");
	insert(&tree, 3, "c");
	insert(&tree, 8, "h");
	insert(&tree, 18, "r");
	insert(&tree, 21, "u");
	insert(&tree, 24, "xx");
	
	return EXIT_SUCCESS;
}