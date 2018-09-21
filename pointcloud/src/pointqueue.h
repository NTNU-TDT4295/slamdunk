#pragma once

#include "linmath.h"
#include <semaphore.h>

struct PointQueue {
	vec3 *points;
	size_t capacity;
	size_t head;

	sem_t lock;

	void push(vec3);
	bool pop(vec3 *out);
};

void point_queue_init(PointQueue &queue, size_t capacity);
void point_queue_free(PointQueue &queue);
