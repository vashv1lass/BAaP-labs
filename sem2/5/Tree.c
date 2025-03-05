#include "Tree.h"

#include "Lab.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum NodeColor NodeColor;

Node * NIL() {
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
	tree->root = NIL();
}

void _Node_rotateLeft(Tree *tree, Node *node) {
	Node *rightChild = node->right;
	
	node->right = rightChild->left;
	if (rightChild->left != NIL()) {
		rightChild->left->parent = node;
	}
	
	rightChild->parent = node->parent;
	if (node->parent == NULL) {
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
	if (leftChild->right != NIL()) {
		leftChild->right->parent = node;
	}
	
	leftChild->parent = node->parent;
	if (node->parent == NULL) {
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
	if (node->parent == NULL) {
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
	if (*currentNode == NIL()) {
		*currentNode = (Node *)malloc(sizeof(Node));

		(*currentNode)->key = key;
		strcpy((*currentNode)->value, value);

		(*currentNode)->left = NIL();
		(*currentNode)->right = NIL();

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
	_Node_insert(tree, &tree->root, NULL, key, value);
}

Node * _Node_subtreeMin(Node *currentNode) {
	if (currentNode->left == NIL()) {
		return currentNode;
	}
	
	return _Node_subtreeMin(currentNode->left);
}

void _Node_eraseRepair(Tree *tree, Node *node) {
	if (node->parent != NULL) {
		Node *parent = node->parent;
		Node *sibling = NULL;
		
		if (node == parent->left) {
			sibling = parent->right;
			
			if (sibling->color == RED) {
				sibling->color = BLACK;
				parent->color = RED;
				_Node_rotateLeft(tree, parent);
				sibling = parent->right;
			}
			
			if (sibling->left->color == BLACK &&
				sibling->right->color == BLACK) {
				sibling->color = RED;
				if (parent->color == RED) {
					parent->color = BLACK;
				} else {
					_Node_eraseRepair(tree, parent);
				}
			} else {
				if (sibling->right->color == BLACK) {
					sibling->left->color = BLACK;
					sibling->color = RED;
					_Node_rotateRight(tree, sibling);
					sibling = parent->right;
				}
				
				sibling->color = parent->color;
				parent->color = BLACK;
				sibling->right->color = BLACK;
				_Node_rotateLeft(tree, parent);
			}
		} else {
			sibling = parent->left;
			
			if (sibling->color == RED) {
				sibling->color = BLACK;
				parent->color = RED;
				_Node_rotateRight(tree, parent);
				sibling = parent->left;
			}
			
			if (sibling->left->color == BLACK &&
				sibling->right->color == BLACK) {
				sibling->color = RED;
				if (parent->color == RED) {
					parent->color = BLACK;
				} else {
					_Node_eraseRepair(tree, parent);
				}
			} else {
				if (sibling->left->color == BLACK) {
					sibling->right->color = BLACK;
					sibling->color = RED;
					_Node_rotateLeft(tree, sibling);
					sibling = parent->left;
				}
				
				sibling->color = parent->color;
				parent->color = BLACK;
				sibling->left->color = BLACK;
				_Node_rotateRight(tree, parent);
			}
		}
	}
}

void _Node_transplant(Tree *tree, Node *u, Node *v) {
	if (u->parent == NULL) {
		tree->root = v;
	} else if (u == u->parent->left) {
		u->parent->left = v;
	} else {
		u->parent->right = v;
	}
	v->parent = u->parent;
}

void _Node_eraseNode(Tree *tree, Node *node) {
	Node *nodeToErase = node;
	Node *successor;
	NodeColor originalColor = nodeToErase->color;
	
	if (node->left == NIL()) {
		successor = node->right;
		_Node_transplant(tree, node, node->right);
	} else if (node->right == NIL()) {
		successor = node->left;
		_Node_transplant(tree, node, node->left);
	} else {
		nodeToErase = _Node_subtreeMin(node->right);
		originalColor = nodeToErase->color;
		successor = nodeToErase->right;
		
		if (nodeToErase->parent == node) {
			successor->parent = nodeToErase;
		} else {
			_Node_transplant(tree, nodeToErase, nodeToErase->right);
			nodeToErase->right = node->right;
			nodeToErase->right->parent = nodeToErase;
		}
		
		_Node_transplant(tree, node, nodeToErase);
		nodeToErase->left = node->left;
		nodeToErase->left->parent = nodeToErase;
		nodeToErase->color = node->color;
	}
	
	if (originalColor == BLACK) {
		_Node_eraseRepair(tree, successor);
	}
	
	free(node);
}

void _Node_erase(Tree *tree, Node **currentNode, int key) {
	if (*currentNode == NIL()) {
		fprintf(stderr, "A node with the key of %d does not exist!\n", key);
	} else {
		if (key < (*currentNode)->key) {
			_Node_erase(tree, &(*currentNode)->left, key);
		} else if (key > (*currentNode)->key) {
			_Node_erase(tree, &(*currentNode)->right, key);
		} else {
			_Node_eraseNode(tree, *currentNode);
		}
	}
}

void erase(Tree *tree, int key) {
	_Node_erase(tree, &tree->root, key);
}

Node * _Node_find(Node *currentNode, int key) {
	if (currentNode == NIL()) {
		return NULL;
	}
	
	if (key < currentNode->key) {
		return _Node_find(currentNode->left, key);
	} else if (key > currentNode->key) {
		return _Node_find(currentNode->right, key);
	}
	return currentNode;
}

const char * find(Tree tree, int key) {
	Node *found = _Node_find(tree.root, key);
	
	if (found == NULL) {
		return NULL;
	}
	return found->value;
}

bool empty(Tree tree) {
	return tree.root == NIL();
}

void _Node_print(Node *currentNode, int currentLevel) {
	if (currentNode != NIL()) {
		currentLevel++;
		
		_Node_print(currentNode->right, currentLevel);
		
		printf("\n");
		for (size_t i = 0; i < currentLevel; i++) {
			printf("\t");
		}
		printf("[%d: %s]\n", currentNode->key, currentNode->value);
		
		_Node_print(currentNode->left, currentLevel);
	}
}

void print(Tree tree) {
	if (tree.root == NIL()) {
		fprintf(stderr, "The tree is empty!\n");
	} else {
		_Node_print(tree.root, 0);
	}
}

void _Node_treeLevelCount(Node *currentNode, int currentLevel, int *levelCount) {
	if (currentNode != NIL()) {
		*levelCount = max(*levelCount, currentLevel);
		
		_Node_treeLevelCount(currentNode->left, currentLevel + 1, levelCount);
		_Node_treeLevelCount(currentNode->right, currentLevel + 1, levelCount);
	}
}

int treeLevelCount(Tree tree) {
	int levelCount = -1;
	_Node_treeLevelCount(tree.root, 0, &levelCount);
	return levelCount + 1;
}