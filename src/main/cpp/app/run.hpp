#pragma once
#include "platform.h"

#include <cstdlib>

void *init_platform();
void deinit_platform(void *platform);

void *dalloc(void *platform, size_t size);
void dfree(void *platform, void *ptr);
void dmemcpy(void *platform, void *dst, unsigned char *src, size_t num);
void dmemread(void *platform, unsigned char *dst, void *src, size_t num);
void bwrite(void *platform, unsigned waddr, unsigned wdata);
unsigned bread(void *platform, unsigned raddr);

void run(void *platform);
