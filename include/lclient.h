/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** client API
*/

#ifndef _LCLIENT_H
#define _LCLIENT_H

#include <lsocket.h>
#include <lbuffer.h>
#include <sys/cdefs.h>

typedef struct lblclient_s {
	lsocket_t socket;
	lbuffer_t buffer;
} lclient_t;

int lclient_create(lclient_t *client, size_t buffer_size, const char *address,
		   uint16_t port) __THROW __nonnull((1));
int lclient_create32(lclient_t *client, size_t buffer_size, uint32_t addr, uint16_t port) __THROW
	__nonnull((1));
ssize_t lclient_update(lclient_t *client, int ms_timeout) __THROW __nonnull((1));
void lclient_destroy(lclient_t *client) __THROW __nonnull((1));
#endif /* !_LCLIENT_H_ */
