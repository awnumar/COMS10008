This is a pretty simple TCP socket server. It binds to 0.0.0.0 and listens on a given port (default: 1337). This program is NOT portable as it utilises Unix sockets, and so will only run on Unix-style systems.

Building and running:

    $ gcc -std=c99 -Wall -O3 -pedantic ./server.c -o ./server
    $ ./server [port] (default: 1337)

When the server is running, assuming no errors were given, you can use netcat to test it out:

    $ netcat 127.0.0.1 1337

Example output on the server side:

    $ ./server
    Received a connection from 127.0.0.1 on port 45706.
    Received a connection from 127.0.0.1 on port 45708.
    Received a connection from 127.0.0.1 on port 45710.

Example output on the client side:

    $ netcat 127.0.0.1 1337
    [message from server]
    $
