#include <iostream>
using namespace std;
#include "platform.h"

#include "EchoNumber.hpp"
void runEchoNumber(WrapperRegDriver *platform)
{
	EchoNumber t(platform);

	unsigned input = 0;
	cout << "Enter a number: " << endl;
	cin >> input;

	cout << "Testing Echonumber.." << endl;
	t.set_dataIn(input);
	t.set_dataWrite(true);

	while(t.get_dataReady() != 1);

	AccelReg output = t.get_dataOut();
	cout << "Output value: " << output << endl;
}

int main()
{
	WrapperRegDriver *platform = initPlatform();
	runEchoNumber(platform);
	deinitPlatform(platform);

	return 0;
}
