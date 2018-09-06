#include "run.hpp"
#include "platform.h"
#include "EchoNumber.hpp"

#include <iostream>

void *init_platform()
{
	return (WrapperRegDriver *) initPlatform();
}

void deinit_platform(void *platform)
{
	deinitPlatform((WrapperRegDriver *) platform);
}

void *dalloc(void *platform, size_t size)
{
	return ((WrapperRegDriver *) platform)->allocAccelBuffer(size);
}

void dfree(void *platform, void *ptr)
{
	((WrapperRegDriver *) platform)->deallocAccelBuffer(ptr);
}

void dmemset(void *platform, void *dst, void *src, unsigned int num)
{
	((WrapperRegDriver *) platform)->
		copyBufferHostToAccel(src, dst, num);
}

void dmemread(void *platform, void *dst, void *src, unsigned int num)
{
	((WrapperRegDriver *) platform)->
		copyBufferAccelToHost(src, dst, num);
}

void bwrite(void *platform, unsigned waddr, unsigned wdata)
{
	EchoNumber t((WrapperRegDriver *) platform);

	t.set_writeAddr(waddr);
	t.set_writeData(wdata);

	t.set_writeEnable(1);
	t.set_writeEnable(0);
}

unsigned bread(void *platform, unsigned raddr)
{
	EchoNumber t((WrapperRegDriver *) platform);

	t.set_readAddr((unsigned int) raddr);
	return t.get_readData();
}

void run(void *platform)
{
	EchoNumber t((WrapperRegDriver *) platform);

	unsigned int nums = 64;
	unsigned int buf_size = nums * sizeof(unsigned int);

	unsigned int *buf = new unsigned int[nums];
	unsigned int *read_buf = new unsigned int[nums];
	for (size_t i = 0; i < nums; ++i) { buf[i] = i + 1; }

	// Move buf into DRAM
	void *dram_read_ptr = dalloc(platform, buf_size);
	void *dram_write_ptr = dalloc(platform, buf_size);
	dmemset(platform, dram_read_ptr, buf, buf_size);

	// Initialize DRAM-module inputs
	t.set_start(0);
	t.set_readBaseAddr((AccelDblReg) dram_read_ptr);
	t.set_writeBaseAddr((AccelDblReg) dram_write_ptr);
	t.set_byteCount(buf_size);

	// Start DRAM module
	t.set_start(1);
	while (t.get_finished() != 1);

	// Fetch the written memory (new location)
	dmemread(platform, read_buf, dram_write_ptr, buf_size);
	for (size_t i = 0; i < nums; ++i) { std::cout << read_buf[i] << std::endl; }

	delete[] buf;
	delete[] read_buf;
	dfree(platform, dram_read_ptr);
	dfree(platform, dram_write_ptr);
}

