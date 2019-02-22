/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** set backlog
*/

#include "lserver.h"

static int set_all(lserver_t *server, int new_backlog)
{
  for (size_t i = 0; i < server->listeners.len; ++i) {
    if (listen(server->listeners.arr[i].socket.fd, new_backlog) == -1)
      return (-1);
  }
  return (0);
}

int lserver_set_backlog(lserver_t *server, uint16_t listener_port, int new_backlog)
{
  if (listener_port == 0)
    return (set_all(server, new_backlog));
  for (size_t i = 0; i < server->listeners.len; ++i) {
    if (server->listeners.arr[i].socket.port == listener_port)
      return (listen(server->listeners.arr[i].socket.fd, new_backlog));
  }
  return (-1);
}