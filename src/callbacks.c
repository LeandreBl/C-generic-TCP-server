#include "lserver.h"

void lserver_on_connect(lserver_t *server, void (*tocall)(lclient_t *client, void *data),
                        void *data_ptr)
{
  server->on_connect = tocall;
  server->data_connect = data_ptr;
}

void lserver_on_disconnect(lserver_t *server, void (*tocall)(lclient_t *client, void *data),
                           void *data_ptr)
{
  server->on_disconnect = tocall;
  server->data_disconnect = data_ptr;
}