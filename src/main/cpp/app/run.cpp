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

void dmemcpy(void *platform, void *dst, unsigned char *src, size_t num)
{
	((WrapperRegDriver *) platform)->
		copyBufferHostToAccel(src, dst, num);
}

void dmemread(void *platform, unsigned char *dst, void *src, size_t num)
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

	unsigned input = 0;
	std::cout << "Enter a number: " << std::endl;
	std::cin >> input;

	std::cout << "Testing Echonumber.." << std::endl;
	t.set_dataIn(input);
	t.set_dataWrite(true);

	while(t.get_dataReady() != 1);

	AccelReg output = t.get_dataOut();
	std::cout << "Output value: " << output << std::endl;
}

