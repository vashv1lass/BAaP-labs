// Лабораторная работа №4. Обратная польская запись.
// Все варианты индивидуальных заданий.

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "Stack.h"

#define STRING_BUFFER_MAX_SIZE (1 << 10)

bool isOperation(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/';
}

int getPriority(char op) {
	switch (op) {
		case '+':
		case '-':
			return 2;
		case '*':
		case '/':
			return 3;
		case ')':
			return 1;
		default:
			return (int)nan("");
	}
}

void toRPN(const char *expr, char *dest) {
	CharStack *operations = NULL;
	
	const size_t exprLen = strlen(expr);
	size_t currentDestIndex = 0;
	
	for (size_t i = 0; i < exprLen; i++) {
		if (expr[i] == '(') {
			operations = pushChar(operations, expr[i]);
		} else if (expr[i] == ')') {
			while (getTopChar(operations) != '(') {
				char currentOperation = '\0';
				operations = popChar(operations, &currentOperation);
				
				dest[currentDestIndex++] = currentOperation;
			}
			
			operations = popChar(operations, NULL);
		} else if (isOperation(expr[i])) {
			while (!charStackIsEmpty(operations) &&
				getPriority((char)getTopChar(operations)) >= getPriority(expr[i])) {
				char currentOperation = '\0';
				operations = popChar(operations, &currentOperation);
				
				dest[currentDestIndex++] = currentOperation;
			}
			
			operations = pushChar(operations, expr[i]);
		} else {
			dest[currentDestIndex++] = expr[i];
		}
	}
	while (!charStackIsEmpty(operations)) {
		char currentOperation = '\0';
		operations = popChar(operations, &currentOperation);
		
		dest[currentDestIndex++] = currentOperation;
	}
	
	dest[currentDestIndex] = '\0';
	
	clearCharStack(&operations);
}

float process(char op, float lhs, float rhs) {
	switch (op) {
		case '+':
			return lhs + rhs;
		case '-':
			return lhs - rhs;
		case '*':
			return lhs * rhs;
		case '/':
			return lhs / rhs;
		default:
			return nanf("");
	}
}

float evaluate(const char *expr, const float *values) {
	char rpn[STRING_BUFFER_MAX_SIZE];
	toRPN(expr, rpn);
	
	FloatStack *nums = NULL;

	const size_t rpnExprLen = strlen(rpn);

	for (size_t i = 0; i < rpnExprLen; i++) {
		if (!isOperation(rpn[i])) {
			nums = pushFloat(nums, values[(size_t)(rpn[i] - 'a')]);
		} else {
			float lhs = .0f, rhs = .0f;
			nums = popFloat(nums, &rhs);
			nums = popFloat(nums, &lhs);
			
			nums = pushFloat(nums, process(rpn[i], lhs, rhs));
		}
	}
	
	float result = .0f;
	nums = popFloat(nums, &result);
	
	clearFloatStack(&nums);
	
	return result;
}

void getExpressionVariableNames(const char *expr, char *dest) {
	const size_t exprLen = strlen(expr);
	size_t currentDestIndex = 0;
	
	for (size_t i = 0; i < exprLen; i++) {
		if (!isOperation(expr[i]) && expr[i] != '(' && expr[i] != ')') {
			dest[currentDestIndex++] = expr[i];
		}
	}
	
	dest[currentDestIndex] = '\0';
}

void inputVariables(const char *expr, float *vars) {
	for (size_t i = 0; i < 26; i++) {
		vars[i] = .0f;
	}
	
	char varNames[STRING_BUFFER_MAX_SIZE];
	getExpressionVariableNames(expr, varNames);
	
	size_t variablesCount = strlen(varNames);
	for (size_t i = 0; i < variablesCount; i++) {
		printf("Введите значение переменной %c: ", varNames[i]);
		float currentVar = .0f;
		scanf("%f", &currentVar);
		
		vars[(size_t)(varNames[i] - 'a')] = currentVar;
	}
}

int main(void) {
	puts("Введите математическое выражение, используя переменные "
		"(переменные должны быть строчными латинскими буквами) "
		"и математические операции (\"+\" - \"плюс\", \"-\" - \"минус\", "
		"\"*\" - \"умножить\", \"/\" - \"разделить\"). Выражение "
		"записывается без пробелов:");
	char expression[STRING_BUFFER_MAX_SIZE];
	scanf("%s", expression);
	
	float values[26];
	inputVariables(expression, values);
	
	float evaluated = evaluate(expression, values);
	printf("Значение выражения %s равно: %.3f",
		expression, evaluated);
	
	return 0;
}