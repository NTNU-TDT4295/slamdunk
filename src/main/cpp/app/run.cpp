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
