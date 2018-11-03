#include "my_string.h"
#include "arena.h"
#include <string.h>
#include <stdio.h>

static inline bool is_numeric(int i)
{
	return (i >= '0' && i <= '9');
}

bool operator==(string lhs, string rhs) {
	if (lhs.length != rhs.length) {
		return false;
	}

	for (size_t i = 0; i < lhs.length; i += 1) {
		if (lhs.data[i] != rhs.data[i]) {
			return false;
		}
	}

	return true;
}

bool operator!=(string lhs, string rhs) {
	return !operator==(lhs, rhs);
}

int read_character(string str, uint8_t **it) {
	int result;

	if (*it >= str.data + str.length) {
		return 0;
	}

	result = **it;
	*it += 1;

	return result;
}

bool string_to_uint64(string str, uint64_t *out) {
	uint64_t result = 0;
	bool is_valid = false;

	for (size_t i = 0; i < str.length; ++i) {
		if (is_numeric(str.data[i])) {
			is_valid = true;
			result = result * 10 + str.data[i] - '0';
		} else {
			break;
		}
	}

	if (!is_valid) {
		return false;
	}

	*out = result;

	return true;
}

bool string_to_float(string str, float *out) {
	float result = 0;
	bool is_valid = false;
	uint8_t *it;
	int c = 0;
	float negate = 1.0f;

	it = str.data;

	c = read_character(str, &it);

	if (c == '-') {
		negate = -1.0f;
	} else {
		it = str.data;
	}

	while ((c = read_character(str, &it)) && is_numeric(c)) {
		is_valid = true;
		result = result * 10.0f + (float)(c - '0');
	}

	if (c == '.') {
		float n = 1.0f;
		while ((c = read_character(str, &it)) && is_numeric(c)) {
			n /= 10.0f;
			result += n * (float)(c - '0');
		}
	}

	result *= negate;

	if (!is_valid) {
		return false;
	}

	*out = result;

	return true;
}

bool string_split(string in, string *result, string *rest, int sep) {
	uint8_t *it = in.data;
	int c;

	if (in.length == 0) {
		return false;
	}

	while ((c = read_character(in, &it)) && c != sep);

	result->data = in.data;
	result->length = it - in.data;

	rest->length = in.length - result->length;
	rest->data = it;

	if (c == sep) {
		result->length -= 1;
	}

	return true;
}

string string_duplicate(arena *arena, string orig) {
	string new_string;

	new_string.length = orig.length;
	new_string.data = (uint8_t *)arena_alloc(arena, new_string.length);
	memcpy(new_string.data, orig.data, new_string.length);

	return new_string;
}

string string_duplicate_zeroterminate(arena *arena, string orig) {
	string new_string;

	new_string.length = orig.length;

	// NOTE: arena_alloc automatically zeroes the memory, so the last
	// byte will implicitly be zero.
	new_string.data = (uint8_t *)arena_alloc(arena, new_string.length + 1);
	memcpy(new_string.data, orig.data, new_string.length);

	return new_string;
}

string string_init(const char *text) {
	return string_init(text, strlen(text));
}
