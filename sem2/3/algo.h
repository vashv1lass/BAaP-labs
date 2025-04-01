#ifndef ALGO_H
#define ALGO_H

#include "queue.h"

queue_node * prev_node(queue, queue_node *);

void push_front(queue *, int);
int pop_rear(queue *);

void view_front(queue);
void view_rear(queue);

queue_node *find_min(queue);

#endif // ALGO_H