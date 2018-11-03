#ifndef GAME_OPENGL_H
#define GAME_OPENGL_H

#include "glad/glad.h"

bool opengl_has_error(const char *loc);

GLuint create_shader_from_file(const char *name, GLenum type);
bool link_shader_program(GLuint program);

bool setup_gl_debug_output();

#endif
