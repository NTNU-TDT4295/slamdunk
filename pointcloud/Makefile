GLAD_DIR:=lib/glad
GLAD_OUT_DIR:=build/glad
GLM_DIR:=lib/glm
SRC_DIR := src/

CFLAGS:=-std=c++11 -Wall -pthread -g -I$(GLAD_OUT_DIR)/include/ -I$(GLM_DIR)
LDFLAGS:=-lX11 -lGL -lGLU -ldl

SRC=$(shell find $(SRC_DIR) -name *.cpp)
SRC+= build/glad/glad.o build/glad/glad_glx.o
HEADERS:=$(find $(SRC_DIR) -name *.h)

GLAD:=PYTHONPATH=$(PYTHONPATH):$(GLAD_DIR) python -m glad

all: pc simulator

pc: $(SRC) $(HEADERS) $(GLM_DIR)/glm
	$(CXX) $(CFLAGS) $(SRC) $(LDFLAGS) -o $@

simulator: $(SRC) $(HEADERS) $(GLM_DIR)/glm
	$(CXX) $(CFLAGS) $(SRC) $(LDFLAGS) -DPROGRAM_MODE_SIMULATOR -o $@


$(GLM_DIR)/glm:
	git submodule update --init --depth 1 $(GLM_DIR)


# Fetch and build GLAD
$(GLAD_DIR)/main.py:
	git submodule update --init --depth 1 $(GLAD_DIR)

$(GLAD_OUT_DIR)/src/glad.c: $(GLAD_DIR)/main.py
	$(GLAD) --out-path $(GLAD_OUT_DIR)/ --generator c --spec gl

$(GLAD_OUT_DIR)/src/glad_glx.c: $(GLAD_DIR)/main.py $(GLAD_OUT_DIR)/src/glad.c
	$(GLAD) --out-path $(GLAD_OUT_DIR)/ --generator c --spec glx

$(GLAD_OUT_DIR)/%.o: $(GLAD_OUT_DIR)/src/%.c
	$(CC) -c -I$(GLAD_OUT_DIR)/include/ $< -o $@

clean:
	-rm pc
.PHONY: clean

clean-glad:
	-rm -r $(GLAD_OUT_DIR)
	-rm pc
.PHONY: clean

purge: clean
	rm -r build
	git submodule deinit --all
.PHONY: purge
