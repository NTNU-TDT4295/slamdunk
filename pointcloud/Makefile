BUILD_DIR:=./build
LIB_DIR:=./lib
GLAD_DIR:=$(LIB_DIR)/glad
GLAD_OUT_DIR:=$(BUILD_DIR)/glad
COMMON_DIR:=../slam_common
SRC_DIR:=./src

CFLAGS:=-std=c++11 -Wall -pthread -g -I$(GLAD_OUT_DIR)/include -I$(COMMON_DIR)/src/
LDFLAGS:=-lX11 -lGL -lGLU -ldl -pthread $(COMMON_DIR)/libslamcommon.a

SRC=$(shell find $(SRC_DIR)/ -name *.cpp)
HEADERS:=$(find $(SRC_DIR)/ -name *.h)

GLAD_INCLUDES=$(GLAD_OUT_DIR)/src/glad.c $(GLAD_OUT_DIR)/src/glad_glx.c
GLAD_OBJ=$(GLAD_OUT_DIR)/glad.o $(GLAD_OUT_DIR)/glad_glx.o

GLAD:=PYTHONPATH=$(PYTHONPATH):$(GLAD_DIR) python -m glad

all: pc sim lidar slam slam_vis

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(GLAD_INCLUDES)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CFLAGS) $< -o $@


COMMON_SRC=$(shell find $(SRC_DIR)/ -path "$(SRC_DIR)/bin" -prune -o -name '*.cpp' -print)


# Build point cloud
PC_SRC=$(shell find $(SRC_DIR)/bin/pc/ -name '*.cpp') $(COMMON_SRC)
PC_OBJ=$(PC_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

pc: $(PC_OBJ) $(COMMON_DIR)/libslamcommon.a
	$(CXX) $(PC_OBJ) $(LDFLAGS) -o $@


# Build simulator
SIM_SRC=$(shell find $(SRC_DIR)/bin/sim/ -name '*.cpp') $(COMMON_SRC)
SIM_OBJ=$(SIM_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

sim: $(SIM_OBJ) $(COMMON_DIR)/libslamcommon.a
	$(CXX) $(SIM_OBJ) $(LDFLAGS) -o $@

# Build lidar
LIDAR_SRC=$(shell find $(SRC_DIR)/bin/lidar/ -name '*.cpp') $(COMMON_SRC)
LIDAR_OBJ=$(LIDAR_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

lidar: $(LIDAR_OBJ) $(COMMON_DIR)/libslamcommon.a
	$(CXX) $(LIDAR_OBJ) $(LDFLAGS) -o $@

# Build slam
SLAM_SRC=$(shell find $(SRC_DIR)/bin/slam/ -name '*.cpp') $(COMMON_SRC)
SLAM_OBJ=$(SLAM_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

slam: $(SLAM_OBJ) $(COMMON_DIR)/libslamcommon.a
	$(CXX) $(SLAM_OBJ) $(LDFLAGS) -o $@

# Build slam_vis
SLAM_VIS_SRC=$(shell find $(SRC_DIR)/bin/slam_vis/ -name '*.cpp') $(COMMON_SRC)
SLAM_VIS_OBJ=$(SLAM_VIS_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

slam_vis: $(SLAM_VIS_OBJ) $(COMMON_DIR)/libslamcommon.a
	$(CXX) $(SLAM_VIS_OBJ) $(LDFLAGS) -o $@

$(COMMON_DIR)/libslamcommon.a:
	make -C $(COMMON_DIR)/ --no-print-directory libslamcommon.a


# Fetch and build GLAD
$(GLAD_OUT_DIR)/src/glad.c: $(GLAD_DIR)/main.py
	$(GLAD) --out-path $(GLAD_OUT_DIR)/ --generator c --spec gl

$(GLAD_OUT_DIR)/src/glad_glx.c: $(GLAD_DIR)/main.py $(GLAD_OUT_DIR)/src/glad.c
	$(GLAD) --out-path $(GLAD_OUT_DIR)/ --generator c --spec glx

$(GLAD_OUT_DIR)/%.o: $(GLAD_OUT_DIR)/src/%.c
	$(CC) -c -I$(GLAD_OUT_DIR)/include/ $< -o $@

# $(GLAD_OUT_DIR)/include/%.h: $(GLAD_OUT_DIR)/src/%.c

clean:
	-rm -f pc
	-rm -f sim
	-rm -f slam
	-rm -f lidar
	-rm -f build/*.o
	-rm -rf build/bin/
.PHONY: clean

clean-glad:
	-rm -rf $(GLAD_OUT_DIR)
	-rm -f pc
.PHONY: clean

purge: clean
	rm -rf build
	git submodule deinit --all
.PHONY: purge
