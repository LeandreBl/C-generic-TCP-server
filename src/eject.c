/*
** EPITECH PROJECT, 2018
** lserver API
** File description:
** eject
*/

#include "lserver.h"

int lserver_eject(lserver_t *server, int fd)
{
  for (size_t i = 0; i < server->clients.len; ++i) {
    if (server->clients.arr[i].socket.fd == fd) {
      if (epoll_ctl(server->epoll, EPOLL_CTL_DEL, fd, NULL) == -1)
        return (-1);
      lvector_erase(server->clients, i);
      return (0);
    }
  }
  return (-1);
}
