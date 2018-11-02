#include "run.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
	WrapperRegDriver* platform = reinterpret_cast<WrapperRegDriver*>(init_platform());

	std::string remote_host = "192.168.1.100";
	std::string remote_port = "6000";
	if (argc < 2) {
		std::cout << "Setting default remote_host to " << remote_host << ':' << remote_port << std::endl;
	} else if (argc >= 2) {
		remote_host = std::string(argv[1]);
		if (argc == 3)
			remote_port = std::string(argv[2]);
	}

	std::cout << "Connecting to remote host: " << remote_host << ':' << remote_port << std::endl;

	slamit(platform, remote_host, remote_port);

	deinit_platform(platform);
}
