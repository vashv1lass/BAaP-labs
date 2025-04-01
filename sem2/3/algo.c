#include "algo.h"

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

queue_node * prev_node(queue q, queue_node *target) {
	if (q.front == target || q.front == NULL) {
		return NULL;
	}
	
	queue_node *current = q.front;
	for (; current->next != NULL && current->next != target; current = current->next);
	
	return current;
}

void push_front(queue *q, int info) {
	queue_node *new_node = (queue_node *)malloc(sizeof(queue_node));
	if (new_node == NULL) {
		return;
	}
	
	new_node->info = info;
	new_node->next = q->front;
	
	if (empty(*q)) {
		q->rear = new_node;
	}
	
	q->front = new_node;
}

int pop_rear(queue *q) {
	if (empty(*q)) {
		return INT_MIN;
	}
	
	int result = q->rear->info;
	
	if (q->front == q->rear) {
		free(q->rear);
		q->front = q->rear = NULL;
	} else {
		queue_node *prev = prev_node(*q, q->rear);
		prev->next = NULL;
		free(q->rear);
		q->rear = prev;
	}
	
	return result;
}

void view_front(queue q) {
	if (empty(q)) {
		printf("Очередь пуста!\n");
		return;
	}
	
	queue_node *current = q.front;
	for (; current != q.rear; current = current->next) {
		printf("%d <- ", current->info);
	}
	printf("%d\n", q.rear->info);
}

void view_rear(queue q) {
	if (empty(q)) {
		printf("Очередь пуста!\n");
		return;
	}
	
	queue_node *current = q.rear;
	for (; current != NULL; current = prev_node(q, current)) {
		if (prev_node(q, current) == NULL) {
			printf("%d\n", current->info);
		} else {
			printf("%d -> ", current->info);
		}
	}
}

queue_node * find_min(queue q) {
	if (empty(q)) {
		return NULL;
	}
	
	queue_node *current = q.front;
	queue_node *min = current;
	for (; current != q.rear; current = current->next) {
		if (min->info > current->info) {
			min = current;
		}
	}
	if (min->info > current->info) {
		min = current;
	}
	
	current = q.front;
	int min_cnt = 0;
	for (; current != q.rear; current = current->next) {
		if (min->info == current->info) {
			min_cnt++;
		}
	}
	if (min->info == current->info) {
		min_cnt++;
	}
	
	if (min_cnt != 1) {
		return NULL;
	}
	return min;
}