/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** update
*/

#include <stdlib.h>
#include <sys/epoll.h>
#include <string.h>

#include "lserver.h"

static int listen_and_move(lserver_t *server)
{
	lclient_t *ptr;
	lclient_t *new;
	struct epoll_event evt;

	evt.events = EPOLLIN;
	for (int i = 0; i < server->esize; ++i) {
		ptr = server->events[i].data.ptr;
		if (ptr->socket->backlog > 0) {
			new = malloc(sizeof(*new));
			evt.data.ptr = new;
			if (new == NULL || lclient_create(new, server->client_buffer_size, NULL, 0) == -1
				|| lsocket_accept(ptr->socket, new->socket) == -1
				|| gtab_append(server->clients, new) == -1
				|| epoll_ctl(server->epoll, EPOLL_CTL_ADD, new->socket->fd, &evt) == -1)
				return (-1);
			memmove(&server->events[i], &server->events[i + 1],
				sizeof(*server->events) * (server->esize - i));
			--i;
			--server->esize;
		}
	}
	if (server->esize == 0) {
		free(server->events);
		server->events = NULL;
	}
	return (0);
}

static int reading_clients(lserver_t *server)
{
	lclient_t *ptr;
	ssize_t rd;

	for (int i = 0; i < server->esize; ++i) {
		ptr = server->events[i].data.ptr;
		rd = cbuffer_fdwrite(ptr->buffer, ptr->socket->fd, -1);
		if (rd == -1)
			return (-1);
		if (rd == 0) {
			if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, ptr->socket->fd, NULL) == -1)
				return (-1);
			memmove(&server->events[i], &server->events[i + 1], sizeof(*server->events));
			--server->esize;
			gtab_remove(server->clients, ptr, _client_destructor);
		}
	}
	return (0);
}

int lserver_update(lserver_t *server, int timeout)
{
	size_t maxlen = server->clients->len + server->listeners->len;

	free(server->events);
	server->events = malloc(sizeof(*server->events) * maxlen);
	if (server->events == NULL) {
		server->esize = 0;
		return (-1);
	}
	server->esize = epoll_wait(server->epoll, server->events, maxlen, timeout);
	if (server->esize == -1) {
		server->esize = 0;
		free(server->events);
		server->events = NULL;
		return (-1);
	}
	if (listen_and_move(server) == -1)
		return (-1);
	return (reading_clients(server));
}