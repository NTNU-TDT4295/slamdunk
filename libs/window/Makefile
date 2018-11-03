BUILD_DIR:=./build
LIB_DIR:=./lib
SRC_DIR:=./src

GLAD_DIR:=$(LIB_DIR)/glad
GLAD_OUT_DIR:=$(BUILD_DIR)/glad

GLAD_INCLUDES=$(GLAD_OUT_DIR)/src/glad.c $(GLAD_OUT_DIR)/src/glad_glx.c
GLAD_OBJ=$(GLAD_OUT_DIR)/glad.o $(GLAD_OUT_DIR)/glad_glx.o

GLAD:=PYTHONPATH=$(PYTHONPATH):$(GLAD_DIR) python -m glad

SLAMCOMMON:=../slamcommon/
SLAMCOMMON_INCLUDE:=$(SLAMCOMMON)/src

SRC=$(shell find $(SRC_DIR)/ -name *.cpp)
OBJS=$(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(GLAD_OBJ)

CFLAGS:=-std=c++11 -Wall -pthread -g -I$(GLAD_OUT_DIR)/include -I$(SLAMCOMMON_INCLUDE)
#LDFLAGS:=-lX11 -lGL -lGLU -ldl -pthread

all: libwindow.a

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(GLAD_INCLUDES)
	@mkdir -p $(dir $@)
	$(CXX) -c $(CFLAGS) $< -o $@

libwindow.a: $(OBJS)
	ar rcs $@ $^

# Fetch and build GLAD
$(GLAD_OUT_DIR)/src/glad.c: $(GLAD_DIR)/main.py
	$(GLAD) --out-path $(GLAD_OUT_DIR)/ --generator c --spec gl

$(GLAD_OUT_DIR)/src/glad_glx.c: $(GLAD_DIR)/main.py $(GLAD_OUT_DIR)/src/glad.c
	$(GLAD) --out-path $(GLAD_OUT_DIR)/ --generator c --spec glx

$(GLAD_OUT_DIR)/%.o: $(GLAD_OUT_DIR)/src/%.c
	$(CC) -c -I$(GLAD_OUT_DIR)/include/ $< -o $@

.PHONY: clean
clean:
	-rm -f libwindow.a
	-rm -f build/*.o

.PHONY: clean-glad
clean-glad:
	-rm -rf $(GLAD_OUT_DIR)
	-rm -f pc

.PHONY: purge
purge: clean
	rm -rf build
