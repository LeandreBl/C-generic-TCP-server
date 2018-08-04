/*
** EPITECH PROJECT, 2018
** server API
** File description:
** client
*/

#include <string.h>
#include <stdlib.h>

#include "lclient.h"

int lclient_create(lclient_t *client, lsocket_t *socket, size_t buffer_size)
{
	client->socket = malloc(sizeof(*client->socket));
	client->buffer = malloc(sizeof(*client->buffer));
	if (client->socket == NULL || client->buffer == NULL)
		return (-1);
	if (socket != NULL)
		memcpy(client->socket, socket, sizeof(*client->socket));
	if (cbuffer_create(client->buffer, buffer_size) == -1)
		return (-1);
	return (0);
}

void lclient_destroy(lclient_t *client)
{
	lsocket_destroy(client->socket);
	cbuffer_destroy(client->buffer);
	free(client->buffer);
	free(client->socket);
}