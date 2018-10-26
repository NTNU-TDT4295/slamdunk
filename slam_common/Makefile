BUILD_DIR:=./build
LIB_DIR:=../pointcloud/lib
SRC_DIR:=./src

SRC=$(shell find $(SRC_DIR)/ -name *.cpp)
OBJS=$(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
HEADERS:=$(find $(SRC_DIR)/ -name *.h)

CFLAGS:=-g -O3 -I$(SRC_DIR)

all: libslamcommon.a

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@-mkdir -p $(dir $@)
	$(CXX) -c $(CFLAGS) $< -o $@

libslamcommon.a: $(OBJS)
	ar rcs $@ $^

.PHONY: clean
clean:
	-rm build/ -rf
	-rm libslamcommon.a

