#include "run.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	WrapperRegDriver* platform = reinterpret_cast<WrapperRegDriver*>(init_platform());

	std::string remote_host;
	if (argc < 2) {
		std::cout << "Setting default remote_host to 192.168.1.100" << std::endl;
		remote_host = "192.168.1.100";
	} else {
		remote_host = std::string(argv[1]);
	}

	slamit(platform, remote_host);

	deinit_platform(platform);
}
