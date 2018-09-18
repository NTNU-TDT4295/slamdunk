#ifndef GAME_STRING_H
#define GAME_STRING_H

#include <stdint.h>
#include <stddef.h>

struct string {
	uint8_t *data;
	size_t length;
};

bool operator==(string lhs, string rhs);
bool operator!=(string lhs, string rhs);

int read_character(string str, uint8_t **i);

bool string_to_uint64(string str, uint64_t * out);
bool string_to_float(string str, float * out);

bool string_split(string in, string *result, string *rest, int sep);

struct arena;

string string_duplicate(arena *arena, string orig);
string string_duplicate_zeroterminate(arena *arena, string orig);

constexpr string string_init(const char *text, size_t length) {
	return string { (uint8_t *)text, length };
}

string string_init(const char *text);

#define STR(x) string_init(x, sizeof(x)-1)
#define LIT(x) (int)x.length, (char *)x.data

#endif
