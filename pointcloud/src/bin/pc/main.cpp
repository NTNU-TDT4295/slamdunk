#include "../../window.h"
#include "../../point_cloud.h"

static void pc_init(void **ctx) {
	*ctx = calloc(1, sizeof(PointCloudContext));
	init_point_cloud(*(PointCloudContext *)(*ctx));
}

static void pc_tick(void *ctx, const WindowFrameInfo &frame) {
	tick_point_cloud(*(PointCloudContext *)ctx, frame);
}

static void pc_free(void *ctx) {
	free_point_cloud(*(PointCloudContext *)ctx);
	free(ctx);
}

int main(int argc, char *argv[]) {
	struct WindowProcs win_procs = {0};
	win_procs.init = pc_init;
	win_procs.tick = pc_tick;
	win_procs.free = pc_free;

	win_procs.capture_mouse = true;
	win_procs.name = "Point Cloud";

	return run_window(argc, argv, win_procs);
}
