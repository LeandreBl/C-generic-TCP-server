# lserver
C TCP/IP socket server, with multi listener.


This server uses my other libs (`socket`, `buffer` and `gtab`) to run.

Each `lclient_t` output is usable in each index of `server->clients->i[i]`.
Each index of `server->clients->i[i]` is a `lclient_t` object.

Everything is readable in it. The buffer is writeable using the `cbuffer_t API` on `client->buffer`.

You can totally use the `server->events` buffer, this buffer indexes gives you informations about latests buffer updates, it's size is stored in `server->esize`.

Lserver example:
```C
lserver_t server;
uint16_t ports[] = {
  2121,
  5555,
};
lclient_t *ptr;
char *line;
int val;

/* Start the server on port 2121 and 5555, each client will have a circular buffer of 2048 bytes */
if (lserver_create(&server, ports, 2, 2048) == -1) {
  fprintf(stderr, "Failed to start server\n");
  return (-1);
}

while (1) {
  /* Gives 100 miliseconds to the server to update */
  lserver_update(&server, 100);
  for (int i = 0; i < server->esize; ++i) {
    ptr = server->events[i].data.ptr;

    /* Read from the client buffer and store each byte into <line> until a '\n' or a ':' is encountered */
    cbuffer_getbytes(ptr->buffer, &line, ":\n");
    /* Read 4 bytes from the client buffer, into the int */
    cbuffer_read(ptr->buffer, &val, sizeof(val));

    printf("%s: %d\n", line, val);
    /* Since cbuffer_getbytes() uses MALLOC(3) to set <line>, it needs to be freed */
    free(line);
  }
}

/* Destroy the server, eject all clients, close all listeners, clean all buffers, free all memory */
lserver_destroy(&server);
```
