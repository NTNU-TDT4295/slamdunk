#include <window.h>
#include "slamvis.h"
#include <stdlib.h>

static void slam_vis_init(void **ctx) {
	*ctx = calloc(1, sizeof(SlamVisContext));
	init_slam_vis(*(SlamVisContext *)(*ctx));
}

static void slam_vis_tick(void *ctx, const WindowFrameInfo &frame) {
	tick_slam_vis(*(SlamVisContext *)ctx, frame);
}

static void slam_vis_free(void *ctx) {
	free_slam_vis(*(SlamVisContext *)ctx);
	free(ctx);
}

int main(int argc, char *argv[]) {
	WindowProcs win_procs = {0};
	win_procs.init = slam_vis_init;
	win_procs.tick = slam_vis_tick;
	win_procs.free = slam_vis_free;

	win_procs.name = "Slam Visualizer";

	return run_window(argc, argv, win_procs);
}
