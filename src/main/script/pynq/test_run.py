from _test import ffi, lib


def test_main():
    platform = lib.init_platform()
    lib.run(platform)


if __name__ == '__main__':
    test_main()
