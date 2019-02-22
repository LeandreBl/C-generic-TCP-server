#include <stdlib.h>

#include "lserver.h"

int lserver_add_listener(lserver_t *server, uint16_t port, int backlog)
{
  lclient_t *listener;
  struct epoll_event evt;

  lvector_emplace_back(server->listeners, lclient_create, server->client_buffer_size, NULL, 0);
  listener = lvector_back(server->listeners);
  evt.data.ptr = listener;
  evt.events = EPOLLIN;
  if (listener == NULL
      || lsocket_server(&listener->socket, port, backlog) == -1
      || epoll_ctl(server->epoll, EPOLL_CTL_ADD, listener->socket.fd, &evt) == -1)
    return (-1);
  return (0);
}

int lserver_del_listener(lserver_t *server, uint16_t port)
{
  for (size_t i = 0; i < server->listeners.len; ++i) {
    if (server->listeners.arr[i].socket.port == port) {
      if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, server->listeners.arr[i].socket.fd, NULL) == -1)
        return (-1);
      lvector_erase(server->listeners, i);
      return (0);
    }
  }
  return (-1);
}
