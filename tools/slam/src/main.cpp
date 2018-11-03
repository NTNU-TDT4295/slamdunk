#include "slam.h"
#include <stdlib.h>

int main(int argc, char *argv[]) {
	SlamContext ctx;
	memset(&ctx, 0, sizeof(SlamContext));
	init_slam(ctx);

	while (true) {
		tick_slam(ctx);
	}

	free_slam(ctx);
}
