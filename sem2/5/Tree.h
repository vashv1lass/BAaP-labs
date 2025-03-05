#ifndef TREE_H
#define TREE_H 1

#include "Lab.h"

#include <stdbool.h>

typedef struct Node {
	int key;
	char value[STRING_BUFFER_MAX_SIZE];
	
	struct Node *left;
	struct Node *right;
	
	struct Node *parent;
	
	enum NodeColor {
		RED,
		BLACK
	} color;
} Node;

typedef struct Tree {
	struct Node *root;
} Tree;

struct Node * NIL();

void initialize(Tree *);

void insert(Tree *, int, const char *);
void erase(Tree *, int);

const char * find(Tree, int);

bool empty(Tree);

void print(Tree);

int treeLevelCount(Tree);

#endif // TREE_H