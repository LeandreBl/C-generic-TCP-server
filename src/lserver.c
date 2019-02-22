#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lserver.h"

static void _lserver_lclient_destructor(lclient_t *client)
{
  lsocket_shutdown(&client->socket);
  lclient_destroy(client);
}

static int lserver_setvalues(lserver_t *server, size_t ports_n, size_t client_buffer_size)
{
  memset(server, 0, sizeof(*server));
  server->client_buffer_size = client_buffer_size;
  server->epoll = epoll_create1(EPOLL_CLOEXEC);
  if (server->epoll == -1)
    return (-1);
  lvector_create(server->clients, 8, _lserver_lclient_destructor);
  lvector_create(server->listeners, ports_n, _lserver_lclient_destructor);
  if (server->clients.arr == NULL || server->listeners.arr == NULL)
    return (-1);
  return (0);
}

static bool port_already_pushed(void *vector, const uint16_t port)
{
  lvector(lclient_t) *v = vector;

  for (size_t i = 0; i < v->len; ++i) {
    if (v->arr[i].socket.port == port)
      return (true);
  }
  return (false);
}

int lserver_create(lserver_t *server, const uint16_t *ports, size_t size, size_t client_buffer_size)
{
  struct epoll_event evt;

  if (lserver_setvalues(server, size, client_buffer_size) == -1)
    return (-1);
  for (size_t i = 0; i < size; ++i) {
    if (port_already_pushed(&server->listeners, ports[i]) == true)
      continue;
    lvector_emplace_back(server->listeners, lclient_create, 0, NULL, 0);
    evt.data.ptr = lvector_back(server->listeners);
    evt.events = EPOLLIN;
    if (lsocket_server(&lvector_back(server->listeners)->socket, ports[i], 64) == -1)
      return (-1);
    if (epoll_ctl(server->epoll, EPOLL_CTL_ADD, lvector_back(server->listeners)->socket.fd, &evt) == -1)
      return (-1);
  }
  return (0);
}

void lserver_destroy(lserver_t *server)
{
  lvector_destroy(server->listeners);
  lvector_destroy(server->clients);
  free(server->events);
  close(server->epoll);
}
