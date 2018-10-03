#include "../../window.h"
#include "../../lidar.h"

static void lidar_init(void **ctx) {
	*ctx = calloc(1, sizeof(LidarContext));
	init_lidar(*(LidarContext *)(*ctx));
}

static void lidar_tick(void *ctx, const WindowFrameInfo &frame) {
	tick_lidar(*(LidarContext *)ctx, frame);
}

static void lidar_free(void *ctx) {
	free_lidar(*(LidarContext *)ctx);
	free(ctx);
}

int main(int argc, char *argv[]) {
	struct WindowProcs win_procs;
	win_procs.init = lidar_init;
	win_procs.tick = lidar_tick;
	win_procs.free = lidar_free;

	return run_window(argc, argv, win_procs);
}
