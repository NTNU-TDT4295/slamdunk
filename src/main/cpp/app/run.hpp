#pragma once
#include "platform.h"

void *init_platform();
void deinit_platform(void *platform);

void run(void *platform);
