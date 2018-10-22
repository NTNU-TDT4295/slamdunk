LIDAR_BASE=./lib/rplidar_sdk/sdk
LIDAR_SRC=$(shell find $(LIDAR_BASE)/sdk/src -name *.cpp)

SRC=$(shell find ./src/ -name *.cpp)

all:
	clang++ -std=c++11 -pthread -I$(LIDAR_BASE)/sdk/include -I$(LIDAR_BASE)/sdk/src $(LIDAR_SRC) $(SRC) -o test
