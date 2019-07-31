/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** eject
*/

#include "lserver.h"

int lserver_eject(lserver_t *server, int fd)
{
	lvector_foreach(client, server->clients)
	{
		if (client->socket.fd == fd) {
			if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, fd, NULL) == -1)
				return (-1);
			lvector_erase_from_ptr(server->clients, client);
			return (0);
		}
	}
	return (-1);
}
