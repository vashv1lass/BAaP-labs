#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#define STACK_UNDERFLOW 1

typedef struct Stack Stack;

Stack *push(Stack *, int);
Stack *pop(Stack *, int *);

void setNext(Stack *, Stack *);
Stack *getNext(Stack *);

void clear(Stack **);

bool isEmpty(Stack *);

#endif // STACK_H