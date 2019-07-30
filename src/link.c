#include <stdlib.h>
#include <errno.h>

#include "lserver.h"

static int saddr_null(lsocket_t *socket, int fd)
{
  struct sockaddr *p = (struct sockaddr *)&socket->saddr;
  socklen_t len = sizeof(*p);

  if (getsockname(fd, p, &len) == -1 && errno == ENOTSOCK)
    return -1;
  return 0;
}

int lserver_link(lserver_t *server, int fd)
{
  lclient_t *new;
  struct epoll_event evt;

  lvector_emplace_back(server->clients, lclient_create, server->config.client_buffer_size, NULL, 0);
  new = lvector_back(server->clients);
  new->socket.fd = fd;
  if (saddr_null(&new->socket, fd) == -1)
    return (-1);
  new->socket.is_connected = true;
  new->socket.backlog = 0;
  new->socket.port = htons(new->socket.saddr.sin_port);
  evt.data.ptr = new;
  evt.events = EPOLLIN;
  if (epoll_ctl(server->epoll, EPOLL_CTL_ADD, new->socket.fd, &evt) == -1)
    return (-1);
  return (0);
}
