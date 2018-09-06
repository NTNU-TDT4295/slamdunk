from _test import ffi, lib
import numpy as np


def test_main():
    # Test running EchoNumber
    platform = lib.init_platform()
    lib.run(platform)

    # -- Test allocting DRAM --
    src = np.ones((5,), dtype=np.uint32)
    src_ptr = ffi.cast("unsigned int *", src.ctypes.data)

    dst = np.zeros((5,), dtype=np.uint32)
    dst_ptr = ffi.cast("unsigned int *", dst.ctypes.data)

    # Allocate 4 bytes of DRAM memory, write four ones to it from src,
    # and then read four bytes from DRAM into dst. dst now contains
    # the ones from src
    base = lib.dalloc(platform, 16)
    lib.dmemset(platform, base, src_ptr, 16)
    lib.dmemread(platform, dst_ptr, base, 16)

    # Write out dst to prove that we have fetched memory cells from
    # the DRAM
    print(dst)

    lib.dfree(platform, base)

    # -- Test BRAM -- (not working for now)
    bram_addr = 1024

    lib.bwrite(platform, bram_addr, 128)
    bram_val = lib.bread(platform, bram_addr)

    # Did we receive anything of interest from BRAM?
    print(bram_val)


if __name__ == '__main__':
    test_main()
