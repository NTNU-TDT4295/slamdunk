#pragma once
#include "platform.h"

#include <cstdlib>

void *init_platform();
void deinit_platform(void *platform);

void *dalloc(void *platform, size_t size);
void dfree(void *platform, void *ptr);
void dmeminit(void *platform, void *dst, unsigned char *src, size_t num);
void dmemread(void *platform, unsigned char *dst, void *src, size_t num);

void run(void *platform);
void run_memory_test(void *platform);
