#include "../../window.h"
#include "../../slam.h"
#include <stdlib.h>

static void slam_init(void **ctx) {
	*ctx = calloc(1, sizeof(SlamContext));
	init_slam(*(SlamContext *)(*ctx));
}

static void slam_tick(void *ctx, const WindowFrameInfo &frame) {
	tick_slam(*(SlamContext *)ctx, frame);
}

static void slam_free(void *ctx) {
	free_slam(*(SlamContext *)ctx);
	free(ctx);
}

int main(int argc, char *argv[]) {
	// SlamContext ctx = {0};
	// WindowFrameInfo info = {0};

	// init_slam(ctx);

	// while (true) {
	// 	tick_slam(ctx, info);
	// }

	// free_slam(ctx);

	WindowProcs win_procs = {0};
	win_procs.init = slam_init;
	win_procs.tick = slam_tick;
	win_procs.free = slam_free;

	win_procs.name = "Slam";

	return run_window(argc, argv, win_procs);
}
