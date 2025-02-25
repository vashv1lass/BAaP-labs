#include "Tree.h"

#include "Lab.h"

#include <stdlib.h>
#include <string.h>

struct Node {
	int key;
	char value[STRING_BUFFER_MAX_SIZE];                                    
	
	enum {
		RED,
		BLACK
	} color;
	
	Node *left;
	Node *right;
	
	Node *parent;
	
	enum ChildSide {
		LEFT,
		RIGHT,
		NO_PARENT
	} side;
};

typedef enum ChildSide ChildSide;

Node * _Node_NIL() {
	static Node *nil = NULL;
	
	if (nil == NULL) {
		nil = (Node *)malloc(sizeof(Node));
		
		nil->color = BLACK;
		
		nil->left = NULL;
		nil->right = NULL;
		
		nil->parent = NULL;
		
		nil->side = NO_PARENT;
	}
	
	return nil;
}

void initialize(Tree *tree) {
	tree->root = _Node_NIL();
}

Node * _Node_find(Node *currentNode, int key) {
	if (currentNode == _Node_NIL()) {
		return _Node_NIL();
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
	
	if (found == _Node_NIL()) {
		return NULL;
	}
	return found->value;
}

void _Node_smallRotation(Node **node) {
	Node *parent = (*node)->parent;
	Node *grandparent = parent->parent;
	Node *child = (((*node)->side == LEFT) ? (*node)->right :
                                                 (*node)->left);
	
	if ((*node)->side == LEFT) {
		(*node)->parent = grandparent;
		(*node)->side = RIGHT;
		grandparent->right = *node;
		
		parent->parent = *node;
		parent->side = RIGHT;
		(*node)->right = parent;
		
		if (child != _Node_NIL()) {
			child->parent = parent;
			child->side = LEFT;
		}
		parent->left = child;
	} else {
		(*node)->parent = grandparent;
		(*node)->side = LEFT;
		grandparent->left = *node;
		
		parent->parent = *node;
		parent->side = LEFT;
		(*node)->left = parent;
		
		if (child != _Node_NIL()) {
			child->parent = parent;
			child->side = RIGHT;
		}
		parent->right = child;
	}
}

void _Node_bigRotation(Node **node) {
	Node *parent = (*node)->parent;
	Node *grandparent = parent->parent;
	Node *sibling = (((*node)->side == LEFT) ? parent->right :
                                                   parent->left);
	Node *grandGrandparent = grandparent->parent;
	
	parent->parent = grandGrandparent;
	parent->side = grandparent->side;
	if (grandparent->side == LEFT) {
		grandGrandparent->left = parent;
	} else if (grandparent->side == RIGHT) {
		grandGrandparent->right = parent;
	}
	
	if ((*node)->side == LEFT) {
		grandparent->parent = parent;
		grandparent->side = RIGHT;
		parent->right = grandparent;
		
		if (sibling != _Node_NIL()) {
			sibling->parent = grandparent;
			sibling->side = LEFT;
		}
		grandparent->left = sibling;
	} else {
		grandparent->parent = parent;
		grandparent->side = LEFT;
		parent->left = grandparent;
		
		if (sibling != _Node_NIL()) {
			sibling->parent = grandparent;
			sibling->side = RIGHT;
		}
		grandparent->right = sibling;
	}
}

void _Node_fixInsert(Node **node) {
	if ((*node)->side == NO_PARENT) {
		(*node)->color = BLACK;
	} else {
		Node *parent = (*node)->parent;
		if (parent->color == RED) {
			if (parent->side == NO_PARENT) {
				parent->color = BLACK;
			} else {
				Node *grandparent = parent->parent;
				Node *uncle = ((parent->side == LEFT) ?
                                               grandparent->right :
                                               grandparent->left);
				
				if (uncle->color == RED) {
					grandparent->color = RED;
					
					parent->color = BLACK;
					uncle->color = BLACK;
					
					_Node_fixInsert(&grandparent);
				} else {
					if ((*node)->side != parent->side) {
						_Node_smallRotation(&(*node));
						
						parent = parent->parent;
						*node = parent;
					}
					
					_Node_bigRotation(&(*node));
					
					Node *sibling = grandparent;
					
					sibling->color = RED;
					parent->color = BLACK;
				}
			}
		}
	}
}

void _Node_insert(Node **currentNode, Node *currentParent,
                  ChildSide currentSide,
                  int key, const char *value) {
	if (*currentNode == _Node_NIL()) {
		*currentNode = NULL;
		*currentNode = (Node *)malloc(sizeof(Node));
		
		(*currentNode)->key = key;
		strcpy((*currentNode)->value, value);
		
		(*currentNode)->color = RED;
		
		(*currentNode)->left = _Node_NIL();
		(*currentNode)->right = _Node_NIL();
		
		(*currentNode)->parent = currentParent;
		
		(*currentNode)->side = currentSide;
		
		_Node_fixInsert(&(*currentNode));
	} else {
		if (key < (*currentNode)->key) {
			_Node_insert(&(*currentNode)->left, *currentNode,
                                     LEFT,
                                     key, value);
		} else if (key > (*currentNode)->key) {
			_Node_insert(&(*currentNode)->right, *currentNode,
                                     RIGHT,
                                     key, value);
		}
	}
}

Node * _Node_root(Node *currentNode) {
	if (currentNode->side == NO_PARENT) {
		return currentNode;
	}
	return _Node_root(currentNode->parent);
}

void insert(Tree *tree, int key, const char *value) {
	_Node_insert(&tree->root, NULL, NO_PARENT, key, value);
	tree->root = _Node_root(tree->root);
}