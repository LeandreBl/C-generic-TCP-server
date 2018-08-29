/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** header
*/

#ifndef _LSERVER_H
# define _LSERVER_H

# include <stdint.h>
# include <stdbool.h>
# include <stddef.h>

# include <stdio.h>
# include <sys/epoll.h>

# include <lgtab.h>
# include <lclient.h>

typedef struct lblserver_s {
	gtab_t *listeners;
	gtab_t *clients;
	struct epoll_event *events;
	size_t client_buffer_size;
	int esize;
	int epoll;
} lserver_t;

int lserver_create(lserver_t *server, const uint16_t *ports, size_t size, size_t client_buffer_size);
int lserver_update(lserver_t *server, int timeout);
void lserver_destroy(lserver_t *server);

int lserver_eject(lserver_t *server, int client_fd);
int lserver_link(lserver_t *server, int client_fd, struct sockaddr_in *saddr);

int lserver_add_listener(lserver_t *server, uint16_t listener_port, int backlog);
int lserver_del_listener(lserver_t *server, uint16_t listener_port);

/* if listener_port == 0, set all listeners */
int lserver_set_backlog(lserver_t *server, uint16_t listener_port, int new_backlog);

/* API internal use only */
void _client_destructor(void *ptr);

#endif /* !_LSERVER_H */
