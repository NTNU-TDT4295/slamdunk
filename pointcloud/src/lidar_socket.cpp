#include "lidar_socket.h"
#include <sys/socket.h>
#include <stdio.h>
#include <semaphore.h>

static void lidar_in_client(net_client_context *net_ctx) {
	LidarSocketContext *ctx;
	ctx = (LidarSocketContext *)net_ctx->user_data;

	uint8_t buffer[5];
	size_t bytes_read = 0;
	size_t num_aligns = 0;
	float last_angle = 0;

	while (!net_ctx->should_quit) {
		while (bytes_read < sizeof(buffer)) {
			int err = recv(net_ctx->socket_fd,
						   ((uint8_t *)buffer) + bytes_read,
						   sizeof(buffer) - bytes_read, 0);
			if (err < 0) {
				perror("recv");
				return;
			} else if (err == 0) {
				return;
			}

			bytes_read += err;
		}

		// printf("%2x %2x %2x %2x %2x %2x ",
		// 	   buffer[0], buffer[1], buffer[2],
		// 	   buffer[3], buffer[4], buffer[5]);

		if (buffer[0] != 0xa5) {
			printf("Missalignment...");
			bytes_read = 0;
			for (unsigned int i = 1; i < sizeof(buffer); i++) {
				if (buffer[i] == 0xa5) {
					printf(" offsetting %i", i);
					bytes_read = sizeof(buffer) - i;
					memmove(&buffer[0], &buffer[i], sizeof(buffer) - i);
				}
			}
			printf("\n");
			num_aligns += 1;
			continue;
		}

		uint16_t angle_q;
		float angle;
		float dist;

		angle_q = ((buffer[2] << 8) | (buffer[1]));
		angle = (float) (angle_q >> 1);
		angle = angle / 64.0;

		dist = (float)((buffer[4] << 8) | buffer[3]) / 4.0f;

		// printf("%zu %10f %10f\n", num_aligns, angle, dist);
		bytes_read = 0;

		// printf("%f %f\n", angle, dist);

		if (angle < last_angle - 50.0f) {
			sem_wait(&ctx->lock);
			ctx->scan_data_read_select ^= 1;
			ctx->scan_data_length[ctx->scan_data_read_select ^ 1] = 0;
			sem_post(&ctx->lock);
		}
		last_angle = angle;

		int write_select = ctx->scan_data_read_select ^ 1;

		if (ctx->scan_data_length[write_select] >= scan_data_cap) {
			printf("Passed scan data cap.\n");
			continue;
		}

		ctx->scan_data[write_select][ctx->scan_data_length[write_select]] = vec2(angle, dist);
		ctx->scan_data_length[write_select] += 1;
	}
}

void init_lidar_socket(LidarSocketContext &ctx, const char *node, const char *service) {
	sem_init(&ctx.lock, 0, 1);

	ctx.net_in.user_data = &ctx;
	ctx.net_in.client_callback = lidar_in_client;
	net_init(&ctx.net_in, node, service);
}

void free_lidar_socket(LidarSocketContext &ctx) {
	net_shutdown(&ctx.net_in);
}
