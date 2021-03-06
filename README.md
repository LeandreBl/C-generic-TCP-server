# lserver
This library includes a linux server implementation, and a client one.

It uses my other libraries (`socket`, `buffer` and `lvector`) to run.

Each `lclient_t` is usable in each index of `server.clients.arr[i]`.

Each `struct poll_event::data.ptr` member is a `client_t *`

You can use the `lvector_foreach` API to loop in it

Everything is readable in it. The buffer is writeable using the `cbuffer_t API` on `client->buffer`.

Lserver example:
```C
#include <stdlib.h>
#include <stdio.h>

#include <lserver.h>

static void say_hi(lserver_t *server, lclient_t *client, uint16_t port, void *userdata)
{
	printf("Say hi to '%s' who joined the server on port %u\n", inet_ntoa(client->socket.saddr.sin_addr), port);
}

static void say_goodbye(lserver_t *server, lclient_t *client, void *userdata)
{
	printf("'%s' left the server, goodbye !\n", inet_ntoa(client->socket.saddr.sin_addr));
}

int main(void)
{
  lserver_t server = {0};
  uint16_t ports[] = {
      2121,
      5555,
  };
  struct lserver_config config = {
	  .ports = ports,
	  .ports_count = 2,
	  .client_buffer_size = 65536,
	  .cbs.on_connect_callback = &say_hi,
	  .cbs.on_connect_userdata = NULL,
	  .cbs.on_disconnect_callback = &say_goodbye,
	  .cbs.on_disconnect_userdata = NULL,
	  .backlog = 5
  };
  lclient_t *ptr;
  char *line;

  /* Start the server on port 2121 and 5555, each client will have a circular buffer of 65536 bytes */
  if (lserver_create(&server, &config) == -1) {
    fprintf(stderr, "Failed to start server\n");
    return (-1);
  }

  while (1) {
    /* Gives 100 miliseconds to the server to update */
    lserver_update(&server, 100);

		/* For each event in the server read events */
    lvector_foreach(evt, server.revents) {
      /* implicit cast of the union in 'struct poll_event' into a 'lclient_t *' */
      ptr = evt->data.ptr;

      /* Read from the client buffer and store each byte into <line> until a "\n" is encountered */
      ssize_t rd = lbuffer_getbytes(&ptr->buffer, &line, "\n");

      /* on 'lbuffer_getbytes()' success */
      if (rd > 0) {
        /* removes the \n */
        rd -= 1;
        line[rd] = '\0';

        /* print the length of the line and the line itself */
        printf("Client: [%zd] \"%s\"\n", rd, line);

        /* answer to the client */
        dprintf(ptr->socket.fd, "Hello !");
      }

      /* Since 'lbuffer_getbytes()' uses MALLOC(3) to set 'line' value, it needs to be freed */
      free(line);
    }
  }

  /* Destroy the server, eject all clients, close all listeners, clean all buffers, free all memory */
  lserver_destroy(&server);
  return (0);
}
```

The other part of the library, which is partially used in the server part, includes a client implementation.

```C
int main(void)
{
  lclient_t client;
  char *output;

  /* Connect the client on the address specified as 3rd parameter on port `2121`, with a circular buffer of `4096` bytes
  ** an other function: `lclient_connect32()`, takes a `uint32_t` as ip address
  */
  lclient_create(&client, 4096, "localhost", 2121);

  /* Reads from the server and store the read bytes into the client's circular buffer and returns the amount of bytes read */
  lclient_update(&client, 100); /* We give 100 milliseconds as timeout to the function */

  /* Retrieve each bytes of the buffer, allocate enough space and copy them into the given pointer */
  lbuffer_retrieve(&client.buffer, &output);

  /* simply printf the output, we expect a human readable null-terminated string */
  printf("%s\n", output);

  /* `lbuffer_retrieve()` uses MALLOC(3), we need to `free()` it */
  free(output);

  /* destroy the client, close the connection and free allocated memory */
  lclient_destroy(&client);
  return (0);
}
```
