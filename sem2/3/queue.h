#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

typedef struct queue_node {
	int info;
	
	struct queue_node *next;
} queue_node;

typedef struct queue {
	queue_node *front;
	queue_node *rear;
} queue;

queue new_queue();

queue copy_queue(queue);

int front(queue);
int rear(queue);

void push(queue *, int);
int pop(queue *);

bool empty(queue);

void delete_queue(queue *);

#endif // QUEUE_H