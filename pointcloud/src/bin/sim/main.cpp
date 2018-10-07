#include "../../window.h"
#include "../../simulator.h"

static void sim_init(void **ctx) {
	*ctx = calloc(1, sizeof(SimulatorContext));
	init_simulator(*(SimulatorContext *)(*ctx));
}

static void sim_tick(void *ctx, const WindowFrameInfo &frame) {
	tick_simulator(*(SimulatorContext *)ctx, frame);
}

static void sim_free(void *ctx) {
	free(ctx);
}

int main(int argc, char *argv[]) {
	struct WindowProcs win_procs = {0};
	win_procs.init = sim_init;
	win_procs.tick = sim_tick;
	win_procs.free = sim_free;

	win_procs.capture_mouse = true;
	win_procs.name = "Simulator";

	return run_window(argc, argv, win_procs);
}
