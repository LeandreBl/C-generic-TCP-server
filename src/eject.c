/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** eject
*/

#include <sys/epoll.h>

#include "lblserver.h"

int lserver_eject(lserver_t *server, int fd)
{
	lclient_t *client;

	for (size_t i = 0; i < server->clients->len; ++i) {
		client = server->clients->i[i];
		if (client->socket->fd == fd) {
			if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, fd, NULL) == -1)
				return (-1);
			gtab_remove_at(server->clients, i, _client_destructor);
			return (0);
		}
	}
	return (-1);
}
