BUILD_DIR:=./build
LIB_DIR:=./lib
GLAD_DIR:=$(LIB_DIR)/glad
GLAD_OUT_DIR:=$(BUILD_DIR)/glad
GLM_DIR:=$(LIB_DIR)/glm
SRC_DIR:=./src

CFLAGS:=-std=c++11 -Wall -pthread -g -I$(GLAD_OUT_DIR)/include -I$(GLM_DIR)
LDFLAGS:=-lX11 -lGL -lGLU -ldl -pthread

SRC=$(shell find $(SRC_DIR)/ -name *.cpp)
HEADERS:=$(find $(SRC_DIR)/ -name *.h)

GLAD_INCLUDES=$(GLAD_OUT_DIR)/src/glad.c $(GLAD_OUT_DIR)/src/glad_glx.c
GLAD_OBJ=$(GLAD_OUT_DIR)/glad.o $(GLAD_OUT_DIR)/glad_glx.o

GLAD:=PYTHONPATH=$(PYTHONPATH):$(GLAD_DIR) python -m glad

all: pc sim lidar slam slam_vis

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(GLM_DIR)/glm $(GLAD_INCLUDES)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CFLAGS) $< -o $@


COMMON_SRC=$(shell find $(SRC_DIR)/ -path "$(SRC_DIR)/bin" -prune -o -name '*.cpp' -print)


# Build point cloud
PC_SRC=$(shell find $(SRC_DIR)/bin/pc/ -name '*.cpp') $(COMMON_SRC)
PC_OBJ=$(PC_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

foo:
	echo $(PC_SRC)
	echo $(PC_OBJ)

pc: $(PC_OBJ)
	$(CXX) $(PC_OBJ) $(LDFLAGS) -o $@


# Build simulator
SIM_SRC=$(shell find $(SRC_DIR)/bin/sim/ -name '*.cpp') $(COMMON_SRC)
SIM_OBJ=$(SIM_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

sim: $(SIM_OBJ)
	$(CXX) $(SIM_OBJ) $(LDFLAGS) -o $@

# Build lidar
LIDAR_SRC=$(shell find $(SRC_DIR)/bin/lidar/ -name '*.cpp') $(COMMON_SRC)
LIDAR_OBJ=$(LIDAR_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

lidar: $(LIDAR_OBJ)
	$(CXX) $(LIDAR_OBJ) $(LDFLAGS) -o $@

# Build slam
SLAM_SRC=$(shell find $(SRC_DIR)/bin/slam/ -name '*.cpp') $(COMMON_SRC)
SLAM_OBJ=$(SLAM_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

slam: $(SLAM_OBJ)
	$(CXX) $(SLAM_OBJ) $(LDFLAGS) -o $@

# Build slam_vis
SLAM_VIS_SRC=$(shell find $(SRC_DIR)/bin/slam_vis/ -name '*.cpp') $(COMMON_SRC)
SLAM_VIS_OBJ=$(SLAM_VIS_SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

slam_vis: $(SLAM_VIS_OBJ)
	$(CXX) $(SLAM_VIS_OBJ) $(LDFLAGS) -o $@


$(GLM_DIR)/glm:
	git submodule update --init --depth 1 $(GLM_DIR)


# Fetch and build GLAD
$(GLAD_DIR)/main.py:
	echo "checkout glad"
	git submodule update --init --depth 1 $(GLAD_DIR)

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
