#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.h"
#include "arena.h"
#include "glad/glad_glx.h"
#include "opengl.h"
#include "linmath.h"
#include "model_loader_obj.h"
#include "string.h"
#include "octree.h"
#include "window.h"

static bool should_exit = false;

static int context_error_handler(Display *display, XErrorEvent *ev) {
	print_error("glx", "Something went wrong while creating OpenGL context.");
	return 0;
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	Octree point_cloud = {0};
	point_cloud.box.center = { 0.0f, 0.0f, 0.0f };
	point_cloud.box.radius = 100.0f;

	point_cloud.insert({ -1.0f, -1.0f, -1.0f });
	point_cloud.insert({  1.0f, -1.0f, -1.0f });
	point_cloud.insert({ -1.0f,  1.0f, -1.0f });
	point_cloud.insert({  1.0f,  1.0f, -1.0f });

	point_cloud.insert({ -1.0f, -1.0f,  1.0f });
	point_cloud.insert({  1.0f, -1.0f,  1.0f });
	point_cloud.insert({ -1.0f,  1.0f,  1.0f });
	point_cloud.insert({  1.0f,  1.0f,  1.0f });

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
	int window_width = 800, window_height = 600;

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
		0, 0, window_width, window_height, // x, y, width, height
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

	model mdl;
	arena transient;

	arena_init(&transient, MEGABYTE(1));

	if (!load_obj_file(transient, STR("assets/models/room.obj"), &mdl)) {
		print_error("model", "Failed to load model!");
		return -1;
	}

	GLuint triangle_vao, triangle_buffer;

	glGenVertexArrays(1, &triangle_vao);
	glBindVertexArray(triangle_vao);

	glGenBuffers(1, &triangle_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, triangle_buffer);

	vec3 triangle_data[] = {
		{-1.0, -1.0, 0.0f},
		{ 1.0, -1.0, 0.0f},
		{ 0.0,  1.0, 0.0f}
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_data), triangle_data, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	GLuint vshader, fshader, shader,
		uniform_in_matrix, uniform_in_projection_matrix,
		uniform_emission_color, uniform_diffuse_color;

	vshader = create_shader_from_file("assets/shaders/test.vsh", GL_VERTEX_SHADER);
	fshader = create_shader_from_file("assets/shaders/test.fsh", GL_FRAGMENT_SHADER);

	shader = glCreateProgram();

	glAttachShader(shader, vshader);
	glAttachShader(shader, fshader);

	link_shader_program(shader);

	uniform_in_matrix = glGetUniformLocation(shader, "in_matrix");
	uniform_in_projection_matrix = glGetUniformLocation(shader, "in_projection_matrix");
	uniform_emission_color = glGetUniformLocation(shader, "emission_color");
	uniform_diffuse_color = glGetUniformLocation(shader, "diffuse_color");

	glViewport(0, 0, window_width, window_height);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	mat4 projection_matrix;
	vec3 camera_position = {
		0.0f, 1.8f, 0.0f
	};
	float camera_yaw = 0.0f, camera_pitch = 0.0f;

	projection_matrix = glm::perspective(45.0f, (float)window_width/(float)window_height, 0.01f, 100.0f);

	glUseProgram(shader);
	glUniformMatrix4fv(uniform_in_projection_matrix, 1, GL_FALSE, glm::value_ptr(projection_matrix));
	glUseProgram(0);

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

	WindowFrameInfo frame_info = {0};

	while (!should_exit) {
		XEvent event;
		bool key_down = false;
		while (XPending(display) > 0) {
			XNextEvent(display, &event);
			switch (event.type) {
			case ConfigureNotify:
				if (event.xconfigure.width != window_width ||
					event.xconfigure.height != window_height) {
					window_width = event.xconfigure.width;
					window_height = event.xconfigure.height;
				}
				break;

			case ClientMessage:
				if (event.xclient.data.l[0] == (long)WM_DELETE_WINDOW) {
					should_exit = true;
				}
				break;

			case FocusOut:
#define KEYBINDING(name, key) frame_info.keyboard.name = false;
#include "keybindings.h"
#undef KEYBINDING
				break;

			case KeyPress:
				key_down = true;
			case KeyRelease: {
				KeySym keysym;
				unsigned int mods;

				XkbLookupKeySym(display, event.xkey.keycode, 0, &mods, &keysym);
				switch (keysym) {
#define KEYBINDING(name, key) case key: frame_info.keyboard.name = key_down; break;
#include "keybindings.h"
#undef KEYBINDING
				}
			} break;
			}
		}

		int pointer_x, pointer_y, _dc_int;
		unsigned int _dc_uint;
		Window _dc_win;

		XQueryPointer(display, window, &_dc_win, &_dc_win,
					  &_dc_int, &_dc_int,
					  &pointer_x, &pointer_y, &_dc_uint);

		pointer_x -= window_width / 2;
		pointer_y -= window_height / 2;

		XWarpPointer(display, None, window, 0, 0, 0, 0, window_width / 2, window_height / 2);
		XSync(display, False);

		frame_info.mouse.dx = pointer_x;
		frame_info.mouse.dy = pointer_y;

		camera_yaw += (float)frame_info.mouse.dx * 0.005f;
		camera_yaw -= floor(camera_yaw);

		camera_pitch += (float)frame_info.mouse.dy * 0.005f;
		if (camera_pitch > 1.0f) {
			camera_pitch = 1.0f;
		} else if (camera_pitch < -1.0f) {
			camera_pitch = -1.0f;
		}

		vec3 move_delta = {};
		vec3 move_final = {};

		if (frame_info.keyboard.left)     {move_delta.x -= 1.0f;}
		if (frame_info.keyboard.right)    {move_delta.x += 1.0f;}

		if (frame_info.keyboard.forward)  {move_delta.z -= 1.0f;}
		if (frame_info.keyboard.backward) {move_delta.z += 1.0f;}

		move_delta /= 0.5;

		if (move_delta.x != 0.0f) {
			move_final.x += cos(PI*2*camera_yaw)/move_delta.x;
			move_final.z += sin(PI*2*camera_yaw)/move_delta.x;
		}

		if (move_delta.z != 0.0f) {
			move_final.x += -sin(PI*2*camera_yaw)/move_delta.z;
			move_final.z += cos(PI*2*camera_yaw)/move_delta.z;
			move_final.y += sin(PI*camera_pitch)/move_delta.z;
		}

		camera_position += move_final * 0.1f;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader);

		mat4 matrix;

		matrix = mat4(1.0f);

		matrix = glm::rotate(matrix, PI/2 * camera_pitch, vec3(1.0f, 0.0f, 0.0f));
		matrix = glm::rotate(matrix, PI*2 * camera_yaw,   vec3(0.0f, 1.0f, 0.0f));
		matrix = glm::translate(matrix, -camera_position);

		glUniformMatrix4fv(uniform_in_matrix, 1, GL_FALSE, glm::value_ptr(matrix));

		glBindVertexArray(mdl.vao);

		glPointSize(5.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform3f(uniform_diffuse_color, 1.0f, 1.0f, 1.0f);
		glUniform3f(uniform_emission_color, 0.0f, 0.0f, 0.0f);
		glDrawArrays(GL_POINTS, 0, mdl.num_vertex);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUniform3f(uniform_diffuse_color, 0.0f, 0.0f, 0.0f);
		glUniform3f(uniform_emission_color, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLES, 0, mdl.num_vertex);
		glBindVertexArray(0);

		glUseProgram(0);

		glXSwapBuffers(display, window);
	}

	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, gl_context);

	XFree(fb_configs);

	XDestroyWindow(display, window);
	XCloseDisplay(display);
}
