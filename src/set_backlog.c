/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** set backlog
*/

#include "lblserver.h"

static int set_all(lserver_t *server, int new_backlog)
{
	lclient_t *ptr;

	for (size_t i = 0; i < server->listeners->len; ++i) {
		ptr = server->listeners->i[i];
		if (listen(ptr->socket->fd, new_backlog) == -1)
			return (-1);
	}
	return (0);
}

int lserver_set_backlog(lserver_t *server, uint16_t listener_port, int new_backlog)
{
	lclient_t *ptr;

	if (listener_port == 0)
		return (set_all(server, new_backlog));
	for (size_t i = 0; i < server->listeners->len; ++i) {
		ptr = server->listeners->i[i];
		if (ptr->socket->port == listener_port)
			return (listen(ptr->socket->fd, new_backlog));
	}
	return (-1);
}