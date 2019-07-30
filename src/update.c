#include <stdlib.h>
#include <string.h>

#include "lserver.h"

static int is_a_listener(lserver_t *server, lclient_t *ptr)
{
  struct epoll_event evt;
  lclient_t *new;

  lvector_emplace_back(server->clients, lclient_create, server->config.client_buffer_size, NULL, 0);
  new = lvector_back(server->clients);
  evt.events = EPOLLIN;
  evt.data.ptr = new;
  if (lsocket_accept(&ptr->socket, &new->socket) == -1
      || epoll_ctl(server->epoll, EPOLL_CTL_ADD, new->socket.fd, &evt) == -1)
    return (-1);
  if (server->config.cbs.on_connect_callback)
    server->config.cbs.on_connect_callback(server, new, ptr->socket.port, server->config.cbs.on_connect_userdata);
  return (0);
}

static int listen_and_move(lserver_t *server)
{
  lclient_t *ptr;

  lvector_foreach(evt, server->revents) {
    ptr = evt->data.ptr;
    if (ptr->socket.backlog > 0) {
      if (is_a_listener(server, ptr) == -1)
        return (-1);
      lvector_erase_from_ptr(server->revents, evt);
      --evt;
    }
  }
  return (0);
}

static int reading_clients(lserver_t *server)
{
  lclient_t *ptr;
  ssize_t rd;

  lvector_foreach(evt, server->revents) {
    ptr = evt->data.ptr;
    rd = lbuffer_fdwrite(&ptr->buffer, ptr->socket.fd, -1);
    if (rd == -1)
      return (-1);
    if (rd == 0) {
      if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, ptr->socket.fd, NULL) == -1)
        return (-1);
      lvector_erase_from_ptr(server->revents, evt);
      --evt;
      if (server->config.cbs.on_disconnect_callback != NULL)
        server->config.cbs.on_disconnect_callback(server, ptr, server->config.cbs.on_disconnect_userdata);
      lvector_erase_from_ptr(server->clients, ptr);
    }
  }
  return (0);
}

int lserver_update(lserver_t *server, int timeout)
{
  size_t maxlen = server->clients.len + server->listeners.len;
  int size;

  lvector_reserve(server->revents, maxlen);
  size = epoll_wait(server->epoll, lvector_data(server->revents), maxlen, timeout);
  if (size == -1) {
    lvector_clear(server->revents);
    return (-1);
  }
  server->revents.len = size;
  if (listen_and_move(server) == -1)
    return (-1);
  return (reading_clients(server));
}
