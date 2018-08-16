/*
 * This is a simple server written in C. It binds itself to 0.0.0.0 and
 * responds to incoming connections on a given port. Simple connections can be
 * made with netcat on Unix systems.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Networking related headers.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Declare global variables.
int port;

// Declare function signatures.
void serverListen();

int main(int argc, char* argv[]) {
    // Determine if the user wants a custom port.
    if (argc == 2) {
        port = strtol(argv[1], NULL, 10);
    } else if (argc == 1) {
        port = 1337;
    } else {
        fprintf(stderr, "Usage: ./server [port] (default: 1337)\n");
        exit(1);
    }

    // Start listening on all hosts, on the specified port.
    serverListen();
}

void serverListen() {
    // Create the TCP socket.
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        fprintf(stderr, "socket: creation failed [err: %s]\n", strerror(errno));
        exit(1);
    }

    // Prepare the server before binding.
    struct sockaddr_in server;
    server.sin_family = AF_INET;         // IP addresses.
    server.sin_addr.s_addr = INADDR_ANY; // Bind to all hosts.
    server.sin_port = htons(port);       // Listen on given port.

    // Bind our socket.
    if (bind(s, (const struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "socket: could not bind [err: %s]\n", strerror(errno));
        exit(1);
    }

    // Tell the kernel that we're listening on this socket.
    if (listen(s, 32) == -1) {
        fprintf(stderr, "socket: error on listen [err: %s]\n", strerror(errno));
        exit(1);
    }

    // Accept connections.
    for (;;) {
        // Define a structure for the client information.
        struct sockaddr_in client;
        socklen_t client_addrlen = sizeof(client);

        // Wait for and accept a single incoming connection.
        int c = accept(s, (struct sockaddr *) &client, &client_addrlen);
        if (c == -1) {
            fprintf(stderr, "socket: error on accept [err: %s]\n", strerror(errno));
            exit(1);
        }

        // Get some basic connection information.
        char* client_addr = inet_ntoa(client.sin_addr);
        int client_port = ntohs(client.sin_port);
        printf("Received a connection from %s on port %d.\n", client_addr, client_port);

        // Send the client a greeting.
        const char* message = "Oi, don't touch me.\n";
        write(c, message, strlen(message));

        // Close the connection.
        close(c);
    }
}
