#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>

#include "utils.h"
#include "arena.h"
#include "glad/glad_glx.h"
#include "opengl.h"
#include "linmath.h"
#include "model_loader_obj.h"
#include "string.h"
#include "octree.h"
#include "window.h"
#include "point_cloud.h"

constexpr int INITIAL_WINDOW_WIDTH = 800;
constexpr int INITIAL_WINDOW_HEIGHT = 800;

static bool should_exit = false;

static int context_error_handler(Display *display, XErrorEvent *ev) {
	print_error("glx", "Something went wrong while creating OpenGL context.");
	return 0;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Display *display;
	Window root_window, window, glx_window;
	int screen;
	GLXFBConfig *fb_configs;
	GLXFBConfig fb_config;
	XVisualInfo *visual;
	XSetWindowAttributes attributes = {0};
	int attributes_mask;
	Colormap colormap;
	GLXContext gl_context = NULL;
	Atom WM_DELETE_WINDOW;

	display = XOpenDisplay(0);

	if (!display) {
		panic("Could not connect to X server.");
	}

	screen = XDefaultScreen(display);
	root_window = XRootWindow(display, screen);

	if (!gladLoadGLX(display, screen)) {
		panic("Failed to initialize glx!");
	}

	const int fb_attributes[] = {
		GLX_RENDER_TYPE,   GLX_RGBA_BIT,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_DOUBLEBUFFER, True,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 16,
		None
	};

	int fb_config_count;
	fb_configs = glXChooseFBConfig(
		display, screen, fb_attributes, &fb_config_count
	);

	if (!fb_configs) {
		panic("Could not choose frame buffer config.");
	}

	fb_config = fb_configs[0];

	visual = glXGetVisualFromFBConfig(display, fb_config);

	if (!fb_configs) {
		panic("Could not get visual from frame buffer configuration.");
	}

	colormap = XCreateColormap(
		display, root_window, visual->visual, AllocNone
	);

	attributes.colormap = colormap;
	attributes.background_pixmap = None;
	attributes.border_pixmap = None;
	attributes.border_pixel = 0;
	attributes.event_mask =
		  StructureNotifyMask
		| EnterWindowMask
		| LeaveWindowMask
		| ExposureMask
		| ButtonPressMask
		| ButtonReleaseMask
		| OwnerGrabButtonMask
		| KeyPressMask
		| KeyReleaseMask
		| FocusChangeMask;

	attributes_mask =
		  CWColormap
		| CWBorderPixel
		| CWEventMask;

	window = XCreateWindow(
		display,
		root_window,
		0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, // x, y, width, height
		0, // border
		visual->depth,
		InputOutput, // class
		visual->visual,
		attributes_mask,
		&attributes
	);

	if (!window) {
		panic("Could not create X window.");
	}

	int glx_attributes[] = {None};

	glx_window = glXCreateWindow(
		display, fb_config, window, glx_attributes
	);

	if (!glx_window) {
		panic("Could not create GLX window.");
	}

	WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);
	Atom protocols[] = {WM_DELETE_WINDOW};
	XSetWMProtocols(display, window, protocols,
					sizeof(protocols) / sizeof(Atom));


	XMapWindow(display, window);
	XStoreName(display, window, "Game");

	if (GLX_ARB_create_context) {
		int context_attributes[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
			GLX_CONTEXT_MINOR_VERSION_ARB, 0,
			None
		};
		int (*old_error_handler)(Display*, XErrorEvent*);
		old_error_handler = XSetErrorHandler(&context_error_handler);
		gl_context = glXCreateContextAttribsARB(
				display, fb_config, 0, True, context_attributes
			);
		XSync(display, False);
		XSetErrorHandler(old_error_handler);
	}

	if (!gl_context) {
		print_error("glx", "Falling back to glXCreateContext.");
		gl_context = glXCreateContext(
			display, visual, NULL, GL_TRUE
		);
	}

	if (!gl_context) {
		panic("Could not create OpenGL context.");
	}

	bool context_ok = glXMakeCurrent(display, window, gl_context);

	if (!context_ok) {
		panic("Could not activate the OpenGL context.");
	}

	if (!gladLoadGL()) {
		panic("Failed to initialize gl!");
	}

	const GLubyte *version = glGetString(GL_VERSION);
	printf("OpenGL version: %s\n", version);

	setup_gl_debug_output();

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Pixmap blank_cursor_pixmap;
	XColor blank_cursor_dummy_color;
	char blank_cursor_data[1] = {0};
	Cursor blank_cursor;

	blank_cursor_pixmap = XCreateBitmapFromData(display, window, blank_cursor_data, 1, 1);
	if (blank_cursor_pixmap == None) {
		print_error("XCreateBitmapFromData", "Failed to create pixmap for blank cursor.");
	}
	blank_cursor = XCreatePixmapCursor(display,
									   blank_cursor_pixmap,
									   blank_cursor_pixmap,
									   &blank_cursor_dummy_color,
									   &blank_cursor_dummy_color,
									   0, 0);
	XFreePixmap(display, blank_cursor_pixmap);

	blank_cursor = XCreateFontCursor(display, XC_arrow);

	XGrabPointer(display, window, False, 0,
				 GrabModeAsync, GrabModeAsync,
				 window, blank_cursor, CurrentTime);

	WindowFrameInfo frame = {0};
	frame.window.width = INITIAL_WINDOW_WIDTH;
	frame.window.height = INITIAL_WINDOW_HEIGHT;
	frame.window.dimentions_changed = true;

	PointCloudContext point_cloud_context = {};
	init_point_cloud(point_cloud_context);

	Octree *octree = &point_cloud_context.octree_render.octree;
	octree->box.center = { 0.0f, 0.0f, 0.0f };
	octree->box.radius = 10.0f;

	octree->insert({ -1.0f, -1.0f, -1.0f });
	octree->insert({  1.0f, -1.0f, -1.0f });
	octree->insert({ -1.0f,  1.0f, -1.0f });
	octree->insert({  1.0f,  1.0f, -1.0f });

	octree->insert({ -1.0f, -1.0f,  1.0f });
	octree->insert({  1.0f, -1.0f,  1.0f });
	octree->insert({ -1.0f,  1.0f,  1.0f });
	octree->insert({  1.0f,  1.0f,  1.0f });


	while (!should_exit) {
		XEvent event;
		while (XPending(display) > 0) {
			bool key_down = true;

			XNextEvent(display, &event);
			switch (event.type) {
			case ConfigureNotify:
				if (event.xconfigure.width != frame.window.width ||
					event.xconfigure.height != frame.window.height) {
					frame.window.width  = event.xconfigure.width;
					frame.window.height = event.xconfigure.height;
					frame.window.dimentions_changed = true;

					glViewport(0, 0, frame.window.width, frame.window.height);
				}
				break;

			case ClientMessage:
				if (event.xclient.data.l[0] == (long)WM_DELETE_WINDOW) {
					should_exit = true;
				}
				break;

			case EnterNotify:
			case FocusIn:
				frame.window.focused = true;
				break;

			case LeaveNotify:
			case FocusOut:
				frame.window.focused = false;
#define KEYBINDING(name, key) frame.keyboard.name = false;
#include "keybindings.h"
#undef KEYBINDING
				break;

			case KeyRelease:
				key_down = false;

				// Check for key-repeat
				if (XEventsQueued(display, QueuedAfterReading)) {
					XEvent next_event;
					XPeekEvent(display, &next_event);

					if (next_event.type == KeyPress &&
						next_event.xkey.time == event.xkey.time &&
						next_event.xkey.keycode == event.xkey.keycode) {

						break;
					}
				}

			case KeyPress: {
				KeySym keysym;
				unsigned int mods;

				XkbLookupKeySym(display, event.xkey.keycode, 0, &mods, &keysym);
				switch (keysym) {
#define KEYBINDING(name, key) case key: frame.keyboard.name = key_down; break;
#include "keybindings.h"
#undef KEYBINDING
				}
			} break;

			default:
				printf("Skipping event %d\n", event.type);
				break;
			}
		}

		int pointer_x = 0, pointer_y = 0, _dc_int;
		unsigned int _dc_uint;
		Window _dc_win;

		if (frame.window.focused) {
			XQueryPointer(display, window, &_dc_win, &_dc_win,
						&_dc_int, &_dc_int,
						&pointer_x, &pointer_y, &_dc_uint);

			pointer_x -= frame.window.width / 2;
			pointer_y -= frame.window.height / 2;

			XWarpPointer(display, None, window, 0, 0, 0, 0,
						 frame.window.width / 2,
						 frame.window.height / 2);
			XSync(display, False);
		}

		frame.mouse.dx = pointer_x;
		frame.mouse.dy = pointer_y;

		tick_point_cloud(point_cloud_context, frame);

		glXSwapBuffers(display, window);

		frame.window.dimentions_changed = false;
	}

	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, gl_context);

	XFree(fb_configs);

	XDestroyWindow(display, window);
	XCloseDisplay(display);
}
