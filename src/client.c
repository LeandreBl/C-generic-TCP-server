/*
** EPITECH PROJECT, 2018
** server API
** File description:
** client
*/

#include <string.h>
#include <stdlib.h>
#include <sys/select.h>

#include "lclient.h"

static int __call_failed(lclient_t *client)
{
	free(client->socket);
	free(client->buffer);
	return (-1);
}

int lclient_create(lclient_t *client, size_t b_size, const char *url, uint16_t port)
{
	client->socket = malloc(sizeof(*client->socket));
	client->buffer = malloc(sizeof(*client->buffer));
	if (client->socket == NULL || client->buffer == NULL)
		return (-1);
	if ((url != NULL && lsocket_connect(client->socket, url, port) == -1)
		|| cbuffer_create(client->buffer, b_size) == -1)
		return (__call_failed(client));
	return (0);
}

int lclient_create32(lclient_t *client, size_t b_size, uint32_t addr, uint16_t port)
{
	client->socket = malloc(sizeof(*client->socket));
	client->buffer = malloc(sizeof(*client->buffer));
	if (client->socket == NULL || client->buffer == NULL)
		return (-1);
	if (lsocket_connect32(client->socket, addr, port) == -1
		|| cbuffer_create(client->buffer, b_size) == -1)
		return (__call_failed(client));
	return (0);
}

ssize_t lclient_update(lclient_t *client, long ms_timeout)
{
	struct timeval tv;
	fd_set rfds;

	tv.tv_sec = 0;
	tv.tv_usec = ms_timeout;
	FD_ZERO(&rfds);
	FD_SET(client->socket->fd, &rfds);
	if (select(1, &rfds, NULL, NULL, &tv) == 0)
		return (0);
	return (cbuffer_fdwrite(client->buffer, client->socket->fd, -1));
}

void lclient_destroy(lclient_t *client)
{
	lsocket_destroy(client->socket);
	cbuffer_destroy(client->buffer);
	free(client->buffer);
	free(client->socket);
}