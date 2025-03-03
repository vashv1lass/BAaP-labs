#ifndef TREE_H
#define TREE_H 1

typedef struct Tree {
	struct Node *root;
} Tree;

void initialize(Tree *);

void insert(Tree *, int, const char *);

#endif // TREE_H