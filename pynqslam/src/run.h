#pragma once
#include "platform.h"

#include <cstdlib>

void *init_platform();
void deinit_platform(void *platform);

void spi_read_ring(void *platform);
