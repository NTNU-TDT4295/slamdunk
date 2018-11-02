#pragma once
#include "platform.h"

#include <cstdlib>
#include <string>

void *init_platform();
void deinit_platform(void *platform);

void slamit(void *platform, std::string remote_host, std::string port);
