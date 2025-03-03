#include "Tree.h"

#include "Lab.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

Node * _Node_NIL() {
	static Node *nil = NULL;

	if (nil == NULL) {
		nil = (Node *)malloc(sizeof(Node));

		nil->left = NULL;
		nil->right = NULL;

		nil->parent = NULL;

		nil->color = BLACK;
	}

	return nil;
}

void initialize(Tree *tree) {
	tree->root = _Node_NIL();
}

void _Node_rotateLeft(Tree *tree, Node *node) {
	Node *rightChild = node->right;
	
	node->right = rightChild->left;
	if (rightChild->left != _Node_NIL()) {
		rightChild->left->parent = node;
	}
	
	rightChild->parent = node->parent;
	if (node->parent == _Node_NIL()) {
		tree->root = rightChild;
	} else if (node == node->parent->left) {
		node->parent->left = rightChild;
	} else {
		node->parent->right = rightChild;
	}
	
	rightChild->left = node;
	node->parent = rightChild;
}

void _Node_rotateRight(Tree *tree, Node *node) {
	Node *leftChild = node->left;
	
	node->left = leftChild->right;
	if (leftChild->right != _Node_NIL()) {
		leftChild->right->parent = node;
	}
	
	leftChild->parent = node->parent;
	if (node->parent == _Node_NIL()) {
		tree->root = leftChild;
	} else if (node == node->parent->right) {
		node->parent->right = leftChild;
	} else {
		node->parent->left = leftChild;
	}
	
	leftChild->right = node;
	node->parent = leftChild;
}

void _Node_insertRepair(Tree *tree, Node *node) {
	if (node->parent == _Node_NIL()) {
		tree->root->color = BLACK;
	} else {
		if (node->parent->color == RED) {
			Node *uncle = (node->parent == node->parent->parent->left ?
					node->parent->parent->right : node->parent->parent->left);
			if (uncle->color == RED) {
				node->parent->color = BLACK;
				uncle->color = BLACK;
				node->parent->parent->color = RED;
				_Node_insertRepair(tree, node->parent->parent);
			} else {
				if (uncle == node->parent->parent->left) {
					if (node == node->parent->left) {
						node = node->parent;
						_Node_rotateRight(tree, node);
					}
					node->parent->color = BLACK;
					node->parent->parent->color = RED;
					_Node_rotateLeft(tree, node->parent->parent);
				} else {
					if (node == node->parent->right) {
						node = node->parent;
						_Node_rotateLeft(tree, node);
					}
					node->parent->color = BLACK;
					node->parent->parent->color = RED;
					_Node_rotateRight(tree, node->parent->parent);
				}
			}
		}
	}
}

void _Node_insert(Tree *tree, Node **currentNode, Node *currentParent,
				  int key, const char *value) {
	if (*currentNode == _Node_NIL()) {
		*currentNode = (Node *)malloc(sizeof(Node));

		(*currentNode)->key = key;
		strcpy((*currentNode)->value, value);

		(*currentNode)->left = _Node_NIL();
		(*currentNode)->right = _Node_NIL();

		(*currentNode)->parent = currentParent;

		(*currentNode)->color = RED;

		_Node_insertRepair(tree, *currentNode);
	} else {
		if (key < (*currentNode)->key) {
			_Node_insert(tree, &(*currentNode)->left, *currentNode,
						 key, value);
		} else if (key > (*currentNode)->key) {
			_Node_insert(tree, &(*currentNode)->right, *currentNode,
						 key, value);
		} else {
			fprintf(stderr, "A node with the key of %d already exists. "
							"Do you want to replace the value corresponding "
							"to this key? (y/n): ", key);

			char response[STRING_BUFFER_MAX_SIZE];
			do {
				scanf("%s", response);
				if (strcmp(response, "y") == 0) {
					strcpy((*currentNode)->value, value);
					break;
				} else if (strcmp(response, "n") == 0) {
					break;
				}
				fprintf(stderr, "The answer must be \'y\' (stands for \"yes\")"
								" or \'n\' (stands for \"no\"): ");
			} while (true);
		}
	}
}

void insert(Tree *tree, int key, const char *value) {
	_Node_insert(tree, &tree->root, _Node_NIL(), key, value);
}