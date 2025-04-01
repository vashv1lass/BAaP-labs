#include "queue.h"

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

queue new_queue() {
	queue q;
	q.front = NULL;
	q.rear = NULL;
	
	return q;
}

queue copy_queue(queue q) {
	queue new_q = new_queue();
	queue_node *current = q.front;
	while (current != NULL) {
		push(&new_q, current->info);
		current = current->next;
	}
	return new_q;
}

int front(queue q) {
	return q.front->info;
}

int rear(queue q) {
	return q.rear->info;
}

void push(queue *q, int info) {
	queue_node *new_node = (queue_node *)malloc(sizeof(queue_node));
	if (new_node == NULL) {
		return;
	}
	
	new_node->info = info;
	new_node->next = NULL;
	
	if (empty(*q)) {
		q->front = q->rear = new_node;
	} else {
		q->rear->next = new_node;
		q->rear = new_node;
	}
}

int pop(queue *q) {
	if (empty(*q)) {
		return INT_MIN;
	}
	
	queue_node *popped_node = q->front;
	int popped_info = popped_node->info;
	q->front = q->front->next;
	
	if (q->front == NULL) {
		q->rear = NULL;
	}
	
	free(popped_node);
	
	return popped_info;
}

bool empty(queue q) {
	return q.front == NULL;
}

void delete_queue(queue *q) {
	queue_node *current = q->front;
	while (current != NULL) {
		queue_node *removed = current;
		current = current->next;
		free(removed);
	}
	q->front = q->rear = NULL;
}