#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lserver.h"

static void _lserver_lclient_destructor(lclient_t *client)
{
	lsocket_shutdown(&client->socket);
	lclient_destroy(client);
}

static int lserver_setvalues(lserver_t *server)
{
	server->epoll = epoll_create1(EPOLL_CLOEXEC);
	if (server->epoll == -1)
		return (-1);
	lvector_create(server->clients, 8, _lserver_lclient_destructor);
	lvector_create(server->listeners, server->config.ports_count, _lserver_lclient_destructor);
	lvector_create(server->revents, 8, NULL);
	if (server->clients.arr == NULL || server->listeners.arr == NULL)
		return (-1);
	return (0);
}

static bool port_already_pushed(void *vector, const uint16_t port)
{
	lvector(lclient_t) *v = vector;

	lvector_foreach(client, *v)
	{
		if (client->socket.port == port)
			return (true);
	}
	return (false);
}

int lserver_create(lserver_t *server, const struct lserver_config *config)
{
	struct epoll_event evt;

	memset(server, 0, sizeof(*server));
	server->epoll = -1;
	server->config = *config;
	if (lserver_setvalues(server) == -1)
		goto err;
	for (size_t i = 0; i < server->config.ports_count; ++i) {
		if (port_already_pushed(&server->listeners, server->config.ports[i]) == true)
			continue;
		lvector_emplace_back(server->listeners, lclient_create, 0, NULL, 0);
		evt.data.ptr = lvector_back(server->listeners);
		evt.events = EPOLLIN;
		if (lsocket_server(&lvector_back(server->listeners)->socket,
				   server->config.ports[i], server->config.backlog)
		    == -1)
			goto err;
		if (epoll_ctl(server->epoll, EPOLL_CTL_ADD,
			      lvector_back(server->listeners)->socket.fd, &evt)
		    == -1)
			goto err;
	}
	return (0);
err:
	lserver_destroy(server);
	return (-1);
}

void lserver_destroy(lserver_t *server)
{
	lvector_destroy(server->listeners);
	lvector_destroy(server->clients);
	lvector_destroy(server->revents);
	if (server->epoll != -1)
		close(server->epoll);
}
