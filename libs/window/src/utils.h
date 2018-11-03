#ifndef PC_UTILS_H
#define PC_UTILS_H

#include <assert.h>

void print_debug(const char *fmt, ...);
void print_error(const char *tag, const char *fmt, ...);
void panic(const char *fmt, ...);
void panic();

#define myassert(expr) \
	if(!(expr)){ \
		panic("Assertion '" #expr "' failed! (" __FILE__ ":%i)",__LINE__); \
	}

#endif
