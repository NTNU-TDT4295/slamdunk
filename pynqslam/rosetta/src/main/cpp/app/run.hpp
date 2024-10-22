#pragma once
#include "platform.h"

#include <cstdlib>

void *init_platform();
void deinit_platform(void *platform);

void *dalloc(void *platform, size_t size);
void dfree(void *platform, void *ptr);
void dmemset(void *platform, void *dst, void *src, unsigned int num);
void dmemread(void *platform, void *dst, void *src, unsigned int num);

void spi_read_ring(void *platform);

void run(void *platform);

void SPI(void* platform);
