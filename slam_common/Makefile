BUILD_DIR:=./build
LIB_DIR:=../pointcloud/lib
SRC_DIR:=./src

SRC=$(shell find $(SRC_DIR)/ -name *.cpp)
OBJS=$(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
HEADERS:=$(find $(SRC_DIR)/ -name *.h)

# Comment for profiling, leave uncommented for normal operation.
CXXFLAGS=-pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O3 -Wall -mcpu=cortex-a9 -mfpu=neon -funsafe-math-optimizations -mcpu=cortex-a9 -mfpu=neon -funsafe-math-optimizations -std=c++11 -I$(SRC_DIR)

# Uncomment for profiling, leav commented for normal operation.
#CFLAGS:=-g -D PROFILING -O3 -I$(SRC_DIR)

all: libslamcommon.a

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@-mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

libslamcommon.a: $(OBJS)
	ar rcs $@ $^

.PHONY: clean
clean:
	-rm build/ -rf
	-rm libslamcommon.a

