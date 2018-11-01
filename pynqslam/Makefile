BOARD_URI=xilinx@pynq2

SLAM_COMMON_LIB=../slam_common/libslamcommon.a
SLAM_COMMON_INC=../slam_common/src

CXX=g++
CXXFLAGS=-pthread -Wno-unused-result -Wsign-compare -DNDEBUG -g -fwrapv -O3 -Wall -mcpu=cortex-a9 -mfpu=neon -funsafe-math-optimizations -mcpu=cortex-a9 -mfpu=neon -funsafe-math-optimizations -std=c++11 -I$(SLAM_COMMON_INC) -c

LDFLAGS=-pthread -lsds_lib $(SLAM_COMMON_LIB)

SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(SOURCES:src/%.cpp=build/%.o)
EXECUTABLE = slam

# .SUFFIXES: .o .cpp

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE) $(LDFLAGS)

.PHONY: all
all: $(EXECUTABLE)

.PHONY: run
run: $(EXECUTABLE)
	sudo ./$(EXECUTABLE) # sudo needed for /dev/mem

.PHONY: clean
clean:
	$(RM) $(EXECUTABLE)
	$(RM) $(OBJECTS)

.PHONY: deploy
deploy:
	@echo ""
	@echo "Uploading to: $(BOARD_URI)"
	rsync -avz ./ $(BOARD_URI):~/pynqslam
	ssh $(BOARD_URI) "cd ~/pynqslam && make"
