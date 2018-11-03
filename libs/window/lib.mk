LIB_WINDOW_DIR := $(dir $(lastword $(MAKEFILE_LIST)))
LIB_WINDOW := $(LIB_WINDOW_DIR)libwindow.a

LIB_WINDOW_CXXFLAGS := -I$(LIB_WINDOW_DIR)src -I$(LIB_WINDOW_DIR)build/glad/include
LIB_WINDOW_LDFLAGS := -lX11 -lGL -lGLU -ldl -pthread $(LIB_WINDOW)

CXXFLAGS += $(LIB_WINDOW_CXXFLAGS)
LDFLAGS  += $(LIB_WINDOW_LDFLAGS)

LOCAL_DEPENDENCIES+= $(LIB_WINDOW)

$(LIB_WINDOW): $(shell find $(LIB_WINDOW_DIR)src/ -name *.h -print -o -name *.cpp -print)
	$(MAKE) -C $(LIB_WINDOW_DIR)
