#ifndef GAME_CONFIG_PARSER_H
#define GAME_CONFIG_PARSER_H

#include "my_string.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

struct config_file {
	string file;
	FILE *fd;
	string line_data;
	string line;
	size_t line_num;
};

void config_trim_whitespace(config_file *config);
bool config_next_line(config_file *config);
bool config_eat_token(config_file *config, string *token);
bool config_eat_token_uint(config_file *config, uint64_t *out);
bool config_eat_token_float(config_file *config, float *out);
uint64_t config_count_remaining_tokens(config_file *config);

#endif
