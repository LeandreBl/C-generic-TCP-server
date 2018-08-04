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

int lclient_create(lclient_t *client, lsocket_t *socket, size_t buffer_size);
void lclient_destroy(lclient_t *client);
#endif /* !_LCLIENT_H_ */
