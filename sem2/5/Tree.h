#ifndef TREE_H
#define TREE_H

typedef struct Node Node;

typedef struct Tree {
	Node *root;
} Tree;

void initialize(Tree *);

const char * find(Tree, int);

void insert(Tree *, int, const char *);

#endif // TREE_H