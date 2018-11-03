#include "net.h"

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 201112L
#endif

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <assert.h>
#include <signal.h>

#define MAX_CLIENTS 32

struct net_client_thread_context {
	pthread_t thread;
	bool connected;
	struct net_client_context external_ctx;
	struct sockaddr addr;
	socklen_t addr_len;
	struct net_context_internal *global_ctx;
};

struct net_context_internal {
	pthread_t thread;
	int socket_fd;
	bool should_quit;
	net_client_callback client_callback;
	void *user_data;
};

static void *net_run(void *data);
static void *net_run_client(void *data);

int net_init(struct net_context *ctx, const char *node, const char *service)
{
	struct addrinfo hints = {0};
	struct addrinfo *servinfo;
	int socket_fd;

	int err;

	assert(ctx->client_callback);

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(node, service, &hints, &servinfo);
	if (err) {
		perror("getaddrinfo");
		return -1;
	}

	socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_fd == -1) {
		perror("socket");
		return -1;
	}

	int opt = 1;

	err = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if (err) {
		perror("setsockopt");
		return -1;
	}

	err = bind(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);

	if (err) {
		perror("bind");
		return -1;
	}

	freeaddrinfo(servinfo);

	ctx->internal = (net_context_internal *)calloc(1, sizeof(struct net_context_internal));

	ctx->internal->client_callback = ctx->client_callback;
	ctx->internal->user_data = ctx->user_data;
	ctx->internal->socket_fd = socket_fd;

	err = pthread_create(&ctx->internal->thread, NULL, net_run, ctx->internal);
	if (err) {
		perror("pthread_create");
		return -1;
	}

	return 0;
}


int net_shutdown(struct net_context *ctx)
{
	int err;

	ctx->internal->should_quit = true;

	err = pthread_kill(ctx->internal->thread, SIGINT);

	err = pthread_join(ctx->internal->thread, NULL);
	if (err) {
		perror("pthread_join");
		return -1;
	}

	free(ctx->internal);

	return 0;
}

static void *net_run(void *data)
{
	struct net_context_internal *ctx = (struct net_context_internal *)data;

	struct net_client_thread_context *clients;
	clients = (net_client_thread_context *)calloc(MAX_CLIENTS, sizeof(struct net_client_thread_context));

	int err;

	err = listen(ctx->socket_fd, MAX_CLIENTS);
	if (err) {
		perror("listen");
		return NULL;
	}

	while (!ctx->should_quit) {
		size_t client_id = 0;
		bool found = false;

		for (size_t i = 0; i < MAX_CLIENTS; i++) {
			if (!clients[i].connected) {
				client_id = i;
				found = true;
				break;
			}
		}

		if (!found) {
			fprintf(stderr, "No available client slots.\n");
			continue;
		}

		struct net_client_thread_context *client_ctx;
		client_ctx = &clients[client_id];

		client_ctx->external_ctx.should_quit = false;

		client_ctx->addr_len = sizeof(struct sockaddr);
		int new_fd;
		new_fd = accept(ctx->socket_fd, &client_ctx->addr, &client_ctx->addr_len);
		if (new_fd < 0) {
			perror("accept");
			continue;
		}
		client_ctx->external_ctx.socket_fd = new_fd;
		client_ctx->external_ctx.user_data = ctx->user_data;

		client_ctx->connected = true;
		client_ctx->global_ctx = ctx;

		pthread_join(client_ctx->thread, NULL);

		err = pthread_create(&client_ctx->thread, NULL, net_run_client, client_ctx);
		if (err) {
			perror("pthread_create");
			continue;
		}
	}

	for (size_t i = 0; i < MAX_CLIENTS; i++) {
		struct net_client_thread_context *client_ctx;
		client_ctx = &clients[i];
		client_ctx->external_ctx.should_quit = true;
	}

	for (size_t i = 0; i < MAX_CLIENTS; i++) {
		struct net_client_thread_context *client_ctx;
		client_ctx = &clients[i];
		pthread_join(client_ctx->thread, NULL);
	}

	return NULL;
}

static void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in *)sa)->sin_addr);
	} else {
		return &(((struct sockaddr_in6 *)sa)->sin6_addr);
	}
}

static void net_print_address(struct sockaddr *addr)
{
	struct sockaddr_storage *storage;
	storage = (struct sockaddr_storage *)addr;
	char buffer[INET6_ADDRSTRLEN];
	inet_ntop(storage->ss_family, get_in_addr(addr), buffer, sizeof(buffer));
	printf("%s", buffer);
}

static void *net_run_client(void *data)
{
	struct net_client_thread_context *ctx;
	ctx = (struct net_client_thread_context *)data;

	printf("Connected to ");
	net_print_address(&ctx->addr);
	printf(".\n");

	ctx->global_ctx->client_callback(&ctx->external_ctx);

	close(ctx->external_ctx.socket_fd);

	printf("Disconnected from ");
	net_print_address(&ctx->addr);
	printf(".\n");

	ctx->connected = false;
	return NULL;
}

int net_client_connect(const char *node, const char *service) {
	struct addrinfo hints = {0};
	struct addrinfo *servinfo;
	int socket_fd;

	int err;

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	err = getaddrinfo(node, service, &hints, &servinfo);
	if (err) {
		perror("getaddrinfo");
		return -1;
	}

	socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	if (socket_fd == -1) {
		perror("socket");
		return -1;
	}

	err = connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen);
	if (err == -1) {
		perror("connect");
		return -1;
	}

	freeaddrinfo(servinfo);

	return socket_fd;
}
