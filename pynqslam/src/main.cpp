#include "run.h"

int main()
{
	WrapperRegDriver* platform = reinterpret_cast<WrapperRegDriver*>(init_platform());

	spi_read_ring(platform);

	deinit_platform(platform);
}
