#include "run.hpp"
#include "platform.h"
// #include "SPI_Slave.hpp"
#include "SPI_Slave.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

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
	bool network = false;

	std::string hostname = "192.168.1.100";
	std::string port = "6002";

	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent* server;

	// Ev. receive
	// constexpr int buffer_size = 255;
	// char buffer[buffer_size];

	if (network) {
		portno = atoi(port.c_str());
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0) {
			std::cerr << "ERROR opening socket" << std::endl;
			exit(1);
		}
		server = gethostbyname(hostname.c_str());
		if (server == NULL) {
			std::cerr << "ERROR, no such host" << std::endl;
			exit(1);
		}
		bzero((char*)&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		bcopy((char*)server->h_addr,
			  (char*)&serv_addr.sin_addr.s_addr,
			  server->h_length);
		serv_addr.sin_port = htons(portno);
		if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
			std::cerr << "ERROR connecting" << std::endl;
			exit(1);
		}
	}

	//////////////////////////////


	SPI_Slave t((WrapperRegDriver*)platform);
	int read_addr = 0;
	int prev_burst = t.get_lidar_burst_counter();
	int current_burst = t.get_lidar_burst_counter();
	// constexpr int lidar_packet_size = 5;
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

			int quality_byte = 0;
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

			uint8_t* lidar_data_byte = reinterpret_cast<uint8_t*>(&lidar_data);

			if (network) {
				for (int i = 0; i < lidar_burst_size; i += 5) {
					if ((lidar_data[i] >> 2) < 20)
						lidar_data[i] = 0xa5;
					else
						lidar_data[i] = 0xa4;
				}

				n = write(sockfd, lidar_data, lidar_burst_size);
				if (n < 0) {
					std::cerr << "ERROR writing to socket" << std::endl;
					exit(1);
				}
			}

			/////////////////////////////////

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

		// Ev. rx
		// n = read(sockfd, buffer, 255);
		// if (n < 0)
		// 	error("ERROR reading from socket");
		// printf("%s\n", buffer);
	}

	if (network)
		close(sockfd);
}

// /* Allocate DRAM and move memory with DMA */
// void run(void *platform)
// {
// 	SPI_Slave t((WrapperRegDriver *) platform);

// 	unsigned int nums = 64;
// 	unsigned int buf_size = nums * sizeof(unsigned int);

// 	unsigned int *buf = new unsigned int[nums];
// 	unsigned int *read_buf = new unsigned int[nums];
// 	for (size_t i = 0; i < nums; ++i) { buf[i] = i + 1; }

// 	// Move buf into DRAM
// 	void *dram_read_ptr = dalloc(platform, buf_size);
// 	void *dram_write_ptr = dalloc(platform, buf_size);
// 	dmemset(platform, dram_read_ptr, buf, buf_size);

// 	// Initialize DRAM-module inputs
// 	t.set_start(0);
// 	t.set_readBaseAddr((AccelDblReg) dram_read_ptr);
// 	t.set_writeBaseAddr((AccelDblReg) dram_write_ptr);
// 	t.set_byteCount(buf_size);

// 	// Start DRAM module
// 	t.set_start(1);
// 	while (t.get_finished() != 1);

// 	// Fetch the written memory (new location)
// 	dmemread(platform, read_buf, dram_write_ptr, buf_size);
// 	for (size_t i = 0; i < nums; ++i) { std::cout << read_buf[i] << std::endl; }

// 	delete[] buf;
// 	delete[] read_buf;
// 	dfree(platform, dram_read_ptr);
// 	dfree(platform, dram_write_ptr);
// }
