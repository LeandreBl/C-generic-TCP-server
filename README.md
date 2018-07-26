# lserver
C TCP/IP socket server, with multi listener.


This server uses my other libs (`socket`, `buffer` and `gtab`) to run.

Each `lclient_t` output is usable in each index of `server->clients->i[i]`.
Each index of `server->clients->i[i]` is a `lclient_t` object.

Everything is readable in it. The buffer is writeable using the `cbuffer_t API` on `client->buffer`.

You can totally use the `server->events` buffer, this buffer indexes gives you informations about latests buffer updates, it's size is stored in `server->esize`.

For example:
```C
lclient_t *ptr;
char *line;
int val;

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
```

