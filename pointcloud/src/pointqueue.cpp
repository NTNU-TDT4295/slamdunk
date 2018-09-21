#include "pointqueue.h"
#include <stdio.h>

void point_queue_init(PointQueue &queue, size_t capacity) {
	queue.head = 0;
	queue.capacity = capacity;
	queue.points = (vec3 *)calloc(queue.capacity, sizeof(vec3));
	sem_init(&queue.lock, 0, 1);
}

void point_queue_free(PointQueue &queue) {
	queue.head = 0;
	queue.capacity = 0;
	free(queue.points);
	sem_destroy(&queue.lock);
}

void PointQueue::push(vec3 point) {
	sem_wait(&this->lock);

	if (this->head < this->capacity) {
		this->points[this->head] = point;
		this->head += 1;
	} else {
		printf("Queue overflow, dropping data.\n");
	}

	sem_post(&this->lock);
}

bool PointQueue::pop(vec3 *out) {
	sem_wait(&this->lock);
	bool found = false;
	vec3 result = *out;

	if (this->head > 0) {
		found = true;
		result = this->points[this->head - 1];
		this->head -= 1;
	}

	sem_post(&this->lock);

	*out = result;
	return found;
}
