#include "run.h"
#include "platform.h"
#include "SPI_Slave.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

void* init_platform()
{
return (WrapperRegDriver*)initPlatform();
}

void deinit_platform(void* platform)
{
	deinitPlatform((WrapperRegDriver*)platform);
}

void spi_read_ring(void* platform)
{
	SPI_Slave t((WrapperRegDriver*)platform);
	int read_addr = 0;
	int prev_burst = t.get_lidar_burst_counter();
	int current_burst = t.get_lidar_burst_counter();
	constexpr int lidar_burst_size = 1800;
	// constexpr int lidar_data_size = lidar_burst_size / 4;
	// uint32_t lidar_data[lidar_data_size];
	uint8_t lidar_data[lidar_burst_size];
	while (true) {
		current_burst = t.get_lidar_burst_counter();
		if (current_burst != prev_burst) {
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
			// printf("0x%08X 0x%08X\n", *lidar_data, lidar_data[lidar_data_size - 1]);

			uint16_t angle_q;
			float angle;
			float dist;

			for (int i = 0; i < lidar_burst_size; i += 5) {

				angle_q = ((lidar_data[i + 2] << 8) | (lidar_data[i + 1]));
				angle = (float)(angle_q >> 1);
				angle = angle / 64.0f;
				dist = (float)((lidar_data[i + 4] << 8) | lidar_data[i + 3]) / 4.0f;
				// if (i % 50 == 0)
				// 	std::cout << std::endl;
				std::cout << std::setw(3) << static_cast<int>(angle) << ' ' << dist << '\n';
			}
			std::cout << "\n=================================" << std::endl;
		}
	}
}
