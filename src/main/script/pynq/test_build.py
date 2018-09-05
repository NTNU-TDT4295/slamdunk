from cffi import FFI
ffibuilder = FFI()

# string that lists all C types, functions and globals that will be
# accessed/needed within Python
ffibuilder.cdef("""
void *init_platform();
void deinit_platform(void *platform);

void run(void *platform);
""")

ffibuilder.set_source("_test",
"""
#include "EchoNumber.hpp"
#include "run.hpp"
#include "platform.h"
""",

sources=['platform-xlnk.cpp', 'run.cpp'],
libraries=['sds_lib'],
extra_compile_args=['-std=c++11'],
source_extension='.cpp')

if __name__ == '__main__':
    ffibuilder.compile(verbose=True)
