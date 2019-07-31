#ifndef _LSERVER_H
#define _LSERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include <stdio.h>
#include <sys/epoll.h>
#include <sys/cdefs.h>

#include <lclient.h>
#include <lvector.h>

/* forward declaration */
typedef struct lblserver_s lserver_t;

struct lserver_callbacks {
	/* function to call when receiving a new connection (can be NULL) */
	void (*on_connect_callback)(lserver_t *server, lclient_t *client, uint16_t port,
				    void *userdata);

	/* userdata to pass when receiveing a new connection (can be NULL) */
	void *on_connect_userdata;

	/* function to call when receiving a disconnection (can be NULL) */
	void (*on_disconnect_callback)(lserver_t *server, lclient_t *client, void *userdata);

	/* userdata to pass when receiving a disconnection (can be NULL) */
	void *on_disconnect_userdata;
};

/* base server configuration */
struct lserver_config {
	/* ports on which the server should listen onto (can be NULL) */
	uint16_t *ports;

	/* the length of the above buffer */
	size_t ports_count;

	/* the size of each clients circular buffer */
	size_t client_buffer_size;

	/* the server callbacks */
	struct lserver_callbacks cbs;

	/* the number a pending connections that can be handled by the kernel */
	int backlog;
};

/* server internal structure */
struct lblserver_s {
	lvector(lclient_t) listeners;
	lvector(lclient_t) clients;
	lvector(struct epoll_event) revents;
	struct lserver_config config;
	int epoll;
}; /* lserver_t */

/**
 * \fn int lserver_create(lserver_t *server, const struct lserver_config *config)
 * \brief Create a new TCP server instance
 *
 * \param server A server handle to be filled with the new instance
 * \param config The server configuration
 * \return -1 in case of error, 0 otherwise
 */
int lserver_create(lserver_t *server, const struct lserver_config *config) __THROW
	__nonnull((1, 2));

/**
 * \fn int lserver_update(lserver_t *server, int timeout)
 * \brief Updates a server instance
 *
 * \param server A server handle to update
 * \param timeout Specifies the number of miliseconds the function will block if there is no event,
 * -1 to block indefinitely \return -1 in case of error, 0 otherwise
 */
int lserver_update(lserver_t *server, int timeout) __THROW __nonnull((1));

/**
 * \fn int lserver_destroy(lserver_t *server)
 * \brief Destroys a server instance
 *
 * \param server A server handle to destroy
 */
void lserver_destroy(lserver_t *server) __THROW __nonnull((1));

/**
 * \fn int lserver_eject(lserver_t *server, int client_fd)
 * \brief Eject a client from it's file descriptor
 *
 * \param server The server handle that accepted the client
 * \param client_fd The file descriptor of the client to disconnect
 * \return -1 in case of error, 0 otherwise
 */
int lserver_eject(lserver_t *server, int client_fd) __THROW __nonnull((1));

/**
 * \fn int lserver_link(lserver_t *server, int client_fd, struct sockaddr_in *saddr)
 * \brief Attach a file descriptor to the server
 *
 * \param server The server handle to be attached
 * \param client_fd The file descriptor to attach to the server event listener
 * \return -1 in case of error, 0 otherwise
 */
int lserver_link(lserver_t *server, int fd) __THROW __nonnull((1));


int lserver_add_listener(lserver_t *server, uint16_t listener_port, int backlog) __THROW
	__nonnull((1));
int lserver_del_listener(lserver_t *server, uint16_t listener_port) __THROW __nonnull((1));

/* if listener_port == 0, set all listeners */
int lserver_set_backlog(lserver_t *server, uint16_t listener_port, int new_backlog) __THROW
	__nonnull((1));

#endif /* !_LSERVER_H */
