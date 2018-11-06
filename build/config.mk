all:
.PHONY: all

MAKE:=make --no-print-directory
CXX:=g++
CXXFLAGS=-g -O3 -std=c++11 -Wall -fwrapv -funsafe-math-optimizations
LDFLAGS=

LOCAL_DEPENDENCIES:=

ifeq ($(TARGET), pynq)
	CXXFLAGS+= -mcpu=cortex-a9 -mfpu=neon
endif
