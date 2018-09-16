#ifndef STAGE_ARENA_H
#define STAGE_ARENA_H

#include <stdint.h>
#include <stddef.h>

struct arena {
	uint8_t *data;
	uint64_t head;
	uint64_t capacity;
};

int arena_init(arena *arena, size_t capacity);
void *arena_alloc(arena *arena, size_t length);

void arena_print_usage(arena *arena);

template<typename T>
T *arena_alloc(arena *arena) {
	return (T *)arena_alloc(arena, sizeof(T));
}

template<typename T>
T *arena_allocn(arena *arena, size_t n) {
	return (T *)arena_alloc(arena, sizeof(T) * n);
}

#define KILOBYTE(x) (x * 1000)
#define MEGABYTE(x) (KILOBYTE(x) * 1000)

#endif
