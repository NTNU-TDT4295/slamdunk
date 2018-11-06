#include "run.hpp"
#include "platform.h"
#include "SPI_Slave.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

#include "net.h"
#include "hector_slam.h"
#include "slam_vis_net.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>

static volatile int keepRunning = 1;
void INTHandler(int) {
    keepRunning = 0;
}

void* init_platform()
{
	return (WrapperRegDriver*)initPlatform();
}

void deinit_platform(void* platform)
{
	deinitPlatform((WrapperRegDriver*)platform);
}

void slamit(void* platform, std::string remote_host, std::string port)
{
	signal(SIGINT, INTHandler);

	int sockfd;

	sockfd = net_client_connect(remote_host.c_str(), port.c_str());

	SPI_Slave t((WrapperRegDriver*)platform);
	int read_addr = 0;
	int prev_burst = t.get_lidar_burst_counter();
	int current_burst = t.get_lidar_burst_counter();
	constexpr int lidar_burst_size = 1800;
	uint8_t lidar_data[lidar_burst_size];
	vec2 processed[1024];
	int num_processed = 0;
	int last_sent_update = 0;
	bool init_sent = false;
	constexpr int init_cutoff = 230; // Cutoffs for number of points each update
	constexpr int update_cutoff = 80;

	HectorSlam slam;
	hs_init(slam);
	// TODO fix
	slam_vis_send_pose(sockfd, {0.0f, 0.0f, 0.0f});
	slam_vis_send_reset_path(sockfd);
	slam_vis_send_map(sockfd,
					  slam.maps[0].values,
					  slam.maps[0].updateIndex,
					  last_sent_update);
	last_sent_update = slam.maps[0].currentUpdateIndex;
	
	while (keepRunning) {
		current_burst = t.get_lidar_burst_counter();
		if (current_burst != prev_burst && current_burst > 3) {

			if (sockfd == -1) {
				this_thread::sleep_for(chrono::seconds(2));
				std::cout << "Reconnecting" << std::endl;
				sockfd = net_client_connect(remote_host.c_str(), port.c_str());
				if (sockfd > 0)
					std::cout << "Connected" << std::endl;

				last_sent_update = 0;
				slam_vis_send_pose(sockfd, {0.0f, 0.0f, 0.0f});
				slam_vis_send_reset_path(sockfd);
				slam_vis_send_map(sockfd,
								  slam.maps[0].values,
								  slam.maps[0].updateIndex,
								  last_sent_update);
				last_sent_update = slam.maps[0].currentUpdateIndex;
				continue;
			}

			prev_burst = current_burst;

			read_addr = (current_burst % 2 == 0) ? 512 : 0;
			uint32_t read_data = 0;
			uint8_t* read_byte = reinterpret_cast<uint8_t*>(&read_data);

			for (int i = 0; i < lidar_burst_size; i += 4) {
				t.set_read_addr(read_addr);
				read_data = t.get_read_data();

				lidar_data[i] = read_byte[3];
				lidar_data[i + 1] = read_byte[2];
				lidar_data[i + 2] = read_byte[1];
				lidar_data[i + 3] = read_byte[0];

				++read_addr;
			}

			uint16_t angle_q;
			uint8_t quality;
			float angle;
			float dist;
			float scale = 1.0f / slam.maps[0].cellSize;

			for (int i = 0; i < lidar_burst_size; i += 5) {
				int tmpn = num_processed;

				if ((lidar_data[i] & 1) > 0) {
					if (num_processed > (init_sent ? update_cutoff : init_cutoff)) {
						init_sent = true;
						hs_update(slam, processed, num_processed);
						slam_vis_send_pose(sockfd, slam.lastPosition);
						if (sockfd >= 0 && last_sent_update < slam.maps[0].currentUpdateIndex) {
							if (slam_vis_send_map(sockfd,
												  slam.maps[0].values,
												  slam.maps[0].updateIndex,
												  last_sent_update) == 0) {
								last_sent_update = slam.maps[0].currentUpdateIndex;
							} else {
								sockfd = -1;
							}
						}
					}
					num_processed = 0;
				}

				quality = lidar_data[i] >> 2;
				angle_q = ((lidar_data[i + 2] << 8) | (lidar_data[i + 1]));
				angle = (float)(angle_q >> 1);
				angle = angle / 64.0f;

				dist = (float)((lidar_data[i + 4] << 8) | lidar_data[i + 3]) / 4.0f;

				if ((lidar_data[i] & 1) > 0) {
					std::cout << angle << " processed: " << tmpn << std::endl;
				}

				angle = angle * M_PI / 180.0f;
				dist  = (dist / 1000.0f) * scale;
				if ((int)quality > 10) {
					processed[num_processed].x = cos(angle) * dist;
					processed[num_processed].y = sin(angle) * dist;
					++num_processed;
				}

			}
		}
	}

	close(sockfd);
	hs_free(slam);
}

