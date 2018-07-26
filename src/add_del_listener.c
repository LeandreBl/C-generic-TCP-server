/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** add / del_listener
*/

#include <sys/epoll.h>
#include <stdlib.h>

#include "lblserver.h"

int lserver_add_listener(lserver_t *server, uint16_t port, int backlog)
{
	lclient_t *listener = malloc(sizeof(*listener));
	struct epoll_event evt;

	evt.data.ptr = listener;
	evt.events = EPOLLIN;
	if (listener == NULL
		|| lclient_create(listener, NULL, server->client_buffer_size) == -1
		|| lsocket_server(listener->socket, port, backlog) == -1
		|| gtab_append(server->listeners, listener) == -1
		|| epoll_ctl(server->epoll, EPOLL_CTL_ADD, listener->socket->fd, &evt) == -1)
		return (-1);
	return (0);	
}

int lserver_del_listener(lserver_t *server, uint16_t port)
{
	lclient_t *ptr;

	for (size_t i = 0; i < server->listeners->len; ++i) {
		ptr = server->listeners->i[i];
		if (ptr->socket->port == port) {
			if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, ptr->socket->fd, NULL) == -1)
				return (-1);
			gtab_remove_at(server->listeners, i, _client_destructor);
			return (0);
		}
	}
	return (-1);
}