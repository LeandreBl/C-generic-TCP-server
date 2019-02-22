#include <stdlib.h>
#include <string.h>

#include "lserver.h"

static int is_a_listener(lserver_t *server, lclient_t *ptr)
{
  struct epoll_event evt;
  lclient_t *new;

  lvector_emplace_back(server->clients, lclient_create, server->client_buffer_size, NULL, 0);
  new = lvector_back(server->clients);
  evt.events = EPOLLIN;
  evt.data.ptr = new;
  if (lsocket_accept(&ptr->socket, &new->socket) == -1
      || epoll_ctl(server->epoll, EPOLL_CTL_ADD, new->socket.fd, &evt) == -1)
    return (-1);
  if (server->on_connect)
    server->on_connect(new, server->data_connect);
  return (0);
}

static int listen_and_move(lserver_t *server)
{
  lclient_t *ptr;

  for (int i = 0; i < server->esize; ++i) {
    ptr = server->events[i].data.ptr;
    if (ptr->socket.backlog > 0) {
      if (is_a_listener(server, ptr) == -1)
        return (-1);
      memmove(&server->events[i], &server->events[i + 1],
              sizeof(*server->events) * (server->esize - i - 1));
      --server->esize;
      --i;
    }
  }
  if (server->esize == 0) {
    free(server->events);
    server->events = NULL;
  }
  return (0);
}

static size_t erase_client(void *vector, lclient_t *ptr)
{
  lvector(lclient_t) *v = vector;
  size_t i;

  for (i = 0; i < v->len; ++i)
    if (&v->arr[i] == ptr)
      return (i);
  return (i);
}

static int reading_clients(lserver_t *server)
{
  lclient_t *ptr;
  ssize_t rd;

  for (int i = 0; i < server->esize; ++i) {
    ptr = server->events[i].data.ptr;
    rd = lbuffer_fdwrite(&ptr->buffer, ptr->socket.fd, -1);
    if (rd == -1)
      return (-1);
    if (rd == 0) {
      if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, ptr->socket.fd, NULL) == -1)
        return (-1);
      memmove(&server->events[i], &server->events[i + 1], sizeof(*server->events));
      --server->esize;
      if (server->on_disconnect != NULL)
        server->on_disconnect(ptr, server->data_disconnect);
      lvector_erase(server->clients, erase_client(&server->clients, ptr));
    }
  }
  return (0);
}

int lserver_update(lserver_t *server, int timeout)
{
  size_t maxlen = server->clients.len + server->listeners.len;

  server->events = realloc(server->events, sizeof(*server->events) * maxlen);
  if (server->events == NULL) {
    server->esize = 0;
    return (-1);
  }
  server->esize = epoll_wait(server->epoll, server->events, maxlen, timeout);
  if (server->esize == -1) {
    server->esize = 0;
    free(server->events);
    server->events = NULL;
    return (-1);
  }
  if (listen_and_move(server) == -1)
    return (-1);
  return (reading_clients(server));
}
