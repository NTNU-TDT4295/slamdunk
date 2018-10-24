#include "run.hpp"
#include "platform.h"
// #include "SPI_Slave.hpp"
#include "SPI_Slave.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <iomanip>

void* init_platform()
{
	return (WrapperRegDriver *) initPlatform();
}

void deinit_platform(void *platform)
{
	deinitPlatform((WrapperRegDriver *) platform);
}

// void *dalloc(void *platform, size_t size)
// {
// 	return ((WrapperRegDriver *) platform)->allocAccelBuffer(size);
// }

// void dfree(void *platform, void *ptr)
// {
// 	((WrapperRegDriver *) platform)->deallocAccelBuffer(ptr);
// }

// void dmemset(void *platform, void *dst, void *src, unsigned int num)
// {
// 	((WrapperRegDriver *) platform)->
// 		copyBufferHostToAccel(src, dst, num);
// }

// void dmemread(void *platform, void *dst, void *src, unsigned int num)
// {
// 	((WrapperRegDriver *) platform)->
// 		copyBufferAccelToHost(src, dst, num);
// }

// void bwrite(void *platform, unsigned waddr, unsigned wdata)
// {
// 	SPI_Slave t((WrapperRegDriver *) platform);

// 	t.set_writeAddr(waddr);
// 	t.set_writeData(wdata);

// 	t.set_writeEnable(1);
// 	t.set_writeEnable(0);
// }

// unsigned bread(void *platform, unsigned raddr)
// {
// 	SPI_Slave t((WrapperRegDriver *) platform);

// 	t.set_readAddr((unsigned int) raddr);
// 	return t.get_readData();
// }

// unsigned int spi_read_addr(void *platform)
// {
// 	SPI_Slave t((WrapperRegDriver *) platform);

// 	// t.set_readAddr((unsigned int) raddr);
// 	return t.get_readAddr();
// }

// unsigned char spi_read_data(void *platform)
// {
// 	SPI_Slave t((WrapperRegDriver *) platform);

// 	// t.set_readAddr((unsigned int) raddr);
// 	return t.get_read_data();
// }

void spi_read_ring(void *platform)
{
	SPI_Slave t((WrapperRegDriver *) platform);
	int read_addr = 0;
	int prev_burst = t.get_lidar_burst_counter();
	int current_burst = t.get_lidar_burst_counter();
	while (true) {
		// t.set_read_addr(read_addr++);

		current_burst = t.get_lidar_burst_counter();
		if (current_burst != prev_burst) {
			std::cout << "TICKO: ";
			prev_burst = current_burst;

			read_addr = (current_burst % 2 == 0) ? 961 : 449;
			t.set_read_addr(read_addr);
			std::cout << std::hex << std::setw(8) << t.get_read_data() << std::endl;
		}

		// while (read_addr != 512) {
		// 	++read_addr;
		// 	t.set_read_addr(read_addr);
		// 	t.get_read_data();
		// }
		// read_addr = 0;


		// read_addr = (current_burst % 2 == 0) ? : ;
		// t.set_read_addr(read_addr);

		// uint32_t int_data = t.get_read_data();

		// std::cout << "0x" << std::hex << int_data << " = " << std::dec << int_data << std::endl;
		
		// char* char_data = reinterpret_cast<char*>(&int_data);
		// std::cout << "read_addr: " << std::dec << read_addr << ", read_data: ";
		// std::cout << "0x" << std::hex << int_data << "\t= " << std::dec << int_data << std::endl;
		// std::cout << "0x" << std::hex << std::setw(8) << int_data << '\t';
		// for (int i = 3; i >= 0; --i) {
		// 	std::cout << char_data[i] << ' ';
		// }
		// std::cout << std::endl;

		// if (read_addr == 2047)
		// 	read_addr = 0;
		// ++read_addr;
		// read_addr = ++read_addr % 2048;

		// std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
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

// void SPI(void* platform)
// {
// 	SPI_Slave t((WrapperRegDriver *) platform);

// 	// char input;

// 	// int count = 0;
// 	// unsigned int data = 0;
// 	// while (true) {
// 	// 	if (t.get_spi_valid()) {
// 	// 		data = t.get_spi_data();
// 	// 		std::cout << data << std::endl;
// 	// 	}
// 	// 	// std::cout << t.get_spi_data();
// 	// }
// 	// ++count;
// 	// if (count % 10000 == 0) {
// 	// 	std::cout << t.get_spi_data() << std::endl;
// 	// 	count = 0;
// 	// }
// 	// std::cout << t.get_spi_sck() << " ";
// 	// std::cout << t.get_spi_valid();
// }
