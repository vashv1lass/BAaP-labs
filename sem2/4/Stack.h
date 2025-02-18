#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#define STACK_UNDERFLOW 1

typedef struct CharStack CharStack;

CharStack *pushChar(CharStack *, char);
CharStack *popChar(CharStack *, char *);

char getTopChar(const CharStack *);

void clearCharStack(CharStack **);

bool charStackIsEmpty(CharStack *);

typedef struct FloatStack FloatStack;

FloatStack *pushFloat(FloatStack *, float);
FloatStack *popFloat(FloatStack *, float *);

float getTopFloat(const FloatStack *);

void clearFloatStack(FloatStack **);

bool floatStackIsEmpty(FloatStack *);

#endif // STACK_H