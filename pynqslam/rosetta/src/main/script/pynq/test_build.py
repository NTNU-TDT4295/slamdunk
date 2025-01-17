from cffi import FFI
ffibuilder = FFI()

# string that lists all C types, functions and globals that will be
# accessed/needed within Python
ffibuilder.cdef("""
void *init_platform();
void deinit_platform(void *platform);

//void *dalloc(void *platform, size_t size);
//void dfree(void *platform, void *ptr);
//void dmemset(void *platform, void *dst, void *src, unsigned int num);
//void dmemread(void *platform, void *dst, void *src, unsigned int num);
//void bwrite(void *platform, unsigned waddr, unsigned wdata);
//unsigned bread(void *platform, unsigned raddr);
//unsigned int spi_read_addr(void *platform);
//unsigned char spi_read_data(void *platform);
void spi_read_ring(void *platform);

//void run(void *platform);

//void SPI(void* platform);
""")

ffibuilder.set_source("_test",
"""
// #include "DMPROTest.hpp"
#include "SPI_Slave.hpp"
#include "run.hpp"
#include "platform.h"
""",

sources=['platform-xlnk.cpp', 'run.cpp'],
libraries=['sds_lib'],
extra_compile_args=['-std=c++11'],
source_extension='.cpp')

if __name__ == '__main__':
    ffibuilder.compile(verbose=True)
