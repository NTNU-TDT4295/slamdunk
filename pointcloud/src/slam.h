#pragma once

#include "window.h"
#include "lidar_socket.h"

struct InternalSlamContext;

struct SlamContext {
	InternalSlamContext *internal;
	LidarSocketContext lidar_socket;
};

void init_slam(SlamContext &ctx);
void tick_slam(SlamContext &ctx, const WindowFrameInfo &info);
void free_slam(SlamContext &ctx);
