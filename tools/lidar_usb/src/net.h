#ifndef STAGE_NET_H
#define STAGE_NET_H

#include <stdbool.h>

struct net_client_context {
	int socket_fd;
	bool should_quit;
	void *user_data;
};

typedef void(*net_client_callback)(struct net_client_context *);

struct net_context_internal;
struct net_context {
	struct net_context_internal *internal;
	net_client_callback client_callback;
	void *user_data;
};

int net_init(struct net_context *, const char *node, const char *service);
int net_shutdown(struct net_context *);
int net_client_connect(const char *node, const char *service);

#endif
