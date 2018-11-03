#include "octree_loader.h"
#include "config_parser.h"
#include "utils.h"

int octree_load_obj(Octree *octree, const char *filename) {
	struct config_file cfg = {};

	cfg.file = string_init(filename);
	cfg.fd = fopen(filename, "rb");

	if (!cfg.fd) {
		perror("open");
		return -1;
	}

	while (config_next_line(&cfg)) {
		string token;
		config_eat_token(&cfg, &token);

		if (token == STR("v")) {
			float x, y, z;

			if (!config_eat_token_float(&cfg, &x)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}
			if (!config_eat_token_float(&cfg, &y)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}
			if (!config_eat_token_float(&cfg, &z)) {
				print_error("obj", "Invalid statement at line %lu\n", cfg.line_num);
				continue;
			}

			octree->insert({ x, y, z });
		}
	}

	return 0;
}
