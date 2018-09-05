from _test import ffi, lib
import numpy as np


def test_main():
    # Test running EchoNumber
    platform = lib.init_platform()
    lib.run(platform)

    # Test allocting DRAM
    src = np.ones((5,), dtype=np.uint8)
    src_ptr = ffi.cast("unsigned char *", src.ctypes.data)

    dst = np.zeros((5,), dtype=np.uint8)
    dst_ptr = ffi.cast("unsigned char *", dst.ctypes.data)

    # Allocate 4 bytes of DRAM memory, write four ones to it from src,
    # and then read four bytes from DRAM into dst. dst now contains
    # the ones from src
    base = lib.dalloc(platform, 4)
    lib.dmeminit(platform, base, src_ptr, 4)
    lib.dmemread(platform, dst_ptr, base, 4)

    # Write out dst to prove that we have fetched memory cells from
    # the DRAM
    print(dst)

    lib.dfree(platform, base)


if __name__ == '__main__':
    test_main()
