#ifndef _LSERVER_H
# define _LSERVER_H

# include <stdint.h>
# include <stdbool.h>
# include <stddef.h>

# include <stdio.h>
# include <sys/epoll.h>
# include <sys/cdefs.h>

# include <lclient.h>
# include <lvector.h>

typedef struct lblserver_s {
	lvector(lclient_t) listeners;
	lvector(lclient_t) clients;
	void *data_connect;
	void *data_disconnect;
	void (* on_connect)(lclient_t *, void *);
	void (* on_disconnect)(lclient_t *, void *);
	lvector(struct epoll_event) revents;
	size_t client_buffer_size;
	int epoll;
} lserver_t;

int lserver_create(lserver_t *server, const uint16_t *ports, size_t ports_buffer_size, size_t client_buffer_size) __THROW __nonnull((1, 2));
int lserver_update(lserver_t *server, int timeout) __THROW __nonnull((1));
void lserver_destroy(lserver_t *server) __THROW __nonnull((1));

int lserver_eject(lserver_t *server, int client_fd) __THROW __nonnull((1));
int lserver_link(lserver_t *server, int client_fd, struct sockaddr_in *saddr) __THROW __nonnull((1));

void lserver_on_connect(lserver_t *server, void (*tocall)(lclient_t *client, void *data), void *data_ptr) __THROW __nonnull((1, 2));
void lserver_on_disconnect(lserver_t *server, void (*tocall)(lclient_t *client, void *data), void *data_ptr) __THROW __nonnull((1, 2));

int lserver_add_listener(lserver_t *server, uint16_t listener_port, int backlog) __THROW __nonnull((1));
int lserver_del_listener(lserver_t *server, uint16_t listener_port) __THROW __nonnull((1));

/* if listener_port == 0, set all listeners */
int lserver_set_backlog(lserver_t *server, uint16_t listener_port, int new_backlog) __THROW __nonnull((1));

#endif /* !_LSERVER_H */
