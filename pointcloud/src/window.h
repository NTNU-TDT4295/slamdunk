#pragma once

#include <stdint.h>

struct WindowFrameInfo {
	struct {
		// Populate keybindings from keybindings.h
#define KEYBINDING(name, key) bool name : 1;
#include "keybindings.h"
#undef KEYBINDING
	} keyboard;

	struct {
		int dx, dy;
	} mouse;

	struct {
		int width, height;
		bool dimentions_changed;

		bool focused;
	} window;

	uint64_t tick;
};

struct WindowProcs {
	void (*init)(void **);
	void (*tick)(void *, const WindowFrameInfo &);
	void (*free)(void *);
};

int run_window(int argc, char *argv[], WindowProcs procs);
