/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** client API
*/

#ifndef _LCLIENT_H
# define _LCLIENT_H

# include <lsocket.h>
# include <lcbuffer.h>

typedef struct lblclient_s {
	lsocket_t *socket;
	cbuffer_t *buffer;
} lclient_t;

int lclient_create(lclient_t *client, size_t buffer_size, const char *address, uint16_t port);
int lclient_create32(lclient_t *client, size_t b_size, uint32_t addr, uint16_t port);
ssize_t lclient_update(lclient_t *client, long ms_timeout);
void lclient_destroy(lclient_t *client);
#endif /* !_LCLIENT_H_ */
