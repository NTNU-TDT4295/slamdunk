#include "opengl.h"
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "utils.h"

bool opengl_has_error(const char *loc) {
	GLenum errCode;
	const GLubyte *errString;
	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		print_error("gl", "%s: (%i) %s", loc, errCode, errString);
		return true;
	}
	return false;
}

GLuint create_shader_from_file(const char *name, GLenum type) {
	char *shader_code;
	ssize_t shader_code_length;

	FILE *fp = fopen(name, "rb");

	if (!fp) {
		print_error("shader", "Could not open shader file '%s': %s", name, strerror(errno));
		return 0;
	}

	int error;

	error = fseek(fp, 0, SEEK_END);

	if (error == -1) {
		print_error("shader", "Could not load shader '%s' (fseek): %s", name, strerror(errno));
		return 0;
	}

	shader_code_length = ftell(fp);

	if (shader_code_length == -1) {
		print_error("shader", "Could not load shader '%s' (ftell): %s", name, strerror(errno));
		return 0;
	}

	error = fseek(fp, 0, SEEK_SET);

	if (error == -1) {
		print_error("shader", "Could not load shader '%s' (fseek): %s", name, strerror(errno));
		return 0;
	}

	shader_code = (char *)malloc(shader_code_length);

	ssize_t bytes_read;
	bytes_read = fread(shader_code, 1, shader_code_length, fp);

	if (bytes_read < shader_code_length) {
		print_error("shader", "Could not read shader file '%s': %s", name, strerror(errno));
		return 0;
	}

	GLuint shader;

	shader = glCreateShader(type);

	GLint shader_code_length_gl = shader_code_length;
	glShaderSource(shader, 1, (const GLchar **)&shader_code, &shader_code_length_gl);

	glCompileShader(shader);

	return shader;
}

bool link_shader_program(GLuint program) {
	GLint link_status;

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, (int *)&link_status);
	if (link_status == GL_FALSE) {
		GLint log_length;

		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);

		char *log = (char*)calloc(log_length+1, 1);
		glGetProgramInfoLog(program, log_length, &log_length, log);

		print_error("shader", "%.*s", (int)log_length, log);
	}

	if (link_status == GL_FALSE) {
		glDeleteProgram(program);
		program = 0;
	}

	return program;
}

static void opengl_debug_print(GLenum source,
							   GLenum type,
							   GLuint id,
							   GLenum severity,
							   GLsizei length,
							   const char* message,
							   const void* userParam) {
	const char *source_verbose;
	const char *type_verbose;
	const char *severity_verbose;

	switch (source) {
	case GL_DEBUG_SOURCE_API:               source_verbose = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:     source_verbose = "Window system"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:   source_verbose = "Shader compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:       source_verbose = "Third party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:       source_verbose = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER:             source_verbose = "Other"; break;
	default:                                source_verbose = "(unknown)"; break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:               type_verbose = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_verbose = "Deprecated behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_verbose = "Undefined behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         type_verbose = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         type_verbose = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER:               type_verbose = "Other"; break;
	default:                                type_verbose = "(unknown)"; break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:            severity_verbose = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:          severity_verbose = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW:             severity_verbose = "Low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:    severity_verbose = "Notification"; break;
	default:                                severity_verbose = "(unknown)"; break;
	}

	fprintf(stderr, "[opengl] severity=%s source=%s type=%s id=%i\n%.*s\n",
			severity_verbose, source_verbose, type_verbose, id,
			length, message);
}

bool setup_gl_debug_output() {
	if (!GL_ARB_debug_output) {
		return false;
	}

	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
	glDebugMessageCallbackARB(&opengl_debug_print, 0);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	return true;
}
