all:
.PHONY: all

MAKE:=make --no-print-directory

CXX:=g++
CXXFLAGS+=-g -std=c++11 -O3 -DNDEBUG -fwrapv -Wall -mcpu=cortex-a9 -mfpu=neon -funsafe-math-optimizations -mcpu=cortex-a9 -mfpu=neon -funsafe-math-optimizations
