#include "run.hpp"

int main()
{
	WrapperRegDriver* platform = reinterpret_cast<WrapperRegDriver*>(init_platform());

	slamit(platform);

	deinit_platform(platform);
}
