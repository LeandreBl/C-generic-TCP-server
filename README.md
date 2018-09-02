# lserver
This libray include a linux server implementation, and a client one.

This library uses my other libs (`socket`, `buffer` and `gtab`) to run.

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
if (lserver_create(&server, ports, sizeof(ports) / sizeof(*ports), 2048) == -1) {
  fprintf(stderr, "Failed to start server\n");
  return (-1);
}

while (1) {
  /* Gives 100 miliseconds to the server to update */
  lserver_update(&server, 100);
  for (int i = 0; i < server->esize; ++i) {
    ptr = server->events[i].data.ptr;

    /* Read from the client buffer and store each byte into <line> until the ":\n" string is encountered */
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

The other part of the library, which is partially used in the server part, includes a client implementation.

```C
int main(void)
{
  lclient_t client;
  char *output;

  /* Connect the client on the address specified as 3rd parameter on port `21`, with a circular buffer of `4096` bytes */
  lclient_create(&client, 4096, "fakeFTP.doesnotexist", 21); /* an other function: `lclient_connect32`, takes an `uint32_t` */
  
  /* Reads from the server and store the read bytes into the client's circular buffer and returns the amount of bytes read */
  lclient_update(&client, 100); /* We give 100 timeout milliseconds to the function */
  
  /* Retrieve each bytes of the buffer, allocate enough space and copy them into the given pointer */
  cbuffer_retrieve(client->buffer, &output);
  printf("%s\n", output);
  free(output);
  
  /* destroy the client, close the connection and free allocated memory */
  lclient_destroy(&client);
  return (0);
}
```
