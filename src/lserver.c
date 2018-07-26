/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** lserver
*/

#include <stdlib.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "lblserver.h"

int lserver_create(lserver_t *server, uint16_t *ports, size_t size, size_t client_buffer_size)
{
	lclient_t *listener;
	struct epoll_event evt;

	server->esize = 0;
	server->events = NULL;
	server->client_buffer_size = client_buffer_size;
	server->listeners = malloc(sizeof(*server->listeners));
	server->clients = malloc(sizeof(*server->clients));
	server->epoll = epoll_create1(EPOLL_CLOEXEC);
	if (server->listeners == NULL || server->clients == NULL || server->epoll == -1)
		return (-1);
	if (gtab_create(server->clients, 8) == -1 ||
		gtab_create(server->listeners, size) == -1)
		return (-1);
	for (size_t i = 0; i < size; ++i) {
		listener = malloc(sizeof(*listener));
		if (listener == NULL)
			return (-1);
		if (lclient_create(listener, NULL, 0) == -1)
			return (-1);
		evt.data.ptr = listener;
		evt.events = EPOLLIN;
		if (lsocket_server(listener->socket, ports[i], 64) == -1)
			return (-1);
		if (gtab_sappend(server->listeners, listener) == -1 ||
			epoll_ctl(server->epoll, EPOLL_CTL_ADD, listener->socket->fd, &evt) == -1)
			return (-1);
	}
	return (0);
}

void _client_destructor(void *ptr)
{
	lclient_destroy(ptr);
	free(ptr);
}

void lserver_destroy(lserver_t *server)
{
	gtab_destroy(server->listeners, _client_destructor);
	gtab_destroy(server->clients, _client_destructor);
	free(server->listeners);
	free(server->clients);
	free(server->events);
	close(server->epoll);
}