/*
 * This is a simple server written in C. It binds itself to 0.0.0.0 and
 * responds to incoming connections on a given port. Simple connections can be
 * made with netcat on Unix systems.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

// Networking related headers.
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Declare global client structure.
struct conn {
    int sock;                // File descriptor for the socket.
    struct sockaddr_in info; // Struct that holds client information.
};

// Declare function signatures.
int create_server(int port);
void server_listen(int s);
void * client_handle(void * clientptr);
void send_bytes(struct conn * client, const char * message, size_t length);

int main(int argc, char * argv[]) {
    // Declare default port.
    int port = 1337;

    // Determine if the user wants a custom port.
    if (argc == 2) {
        port = strtol(argv[1], NULL, 10);
    } else if (argc == 1) {} else {
        fprintf(stderr, "Usage: ./server [port] (default: 1337)\n");
        exit(1);
    }

    // Bind and create the server.
    int s = create_server(port);

    // Start listening for and handling connections.
    server_listen(s);
}

// Configures, binds, and starts the server socket.
int create_server(int port) {
    // Create the TCP socket.
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        fprintf(stderr, "socket: creation failed [%s]\n", strerror(errno));
        exit(1);
    }

    // Prepare the server before binding.
    struct sockaddr_in server;
    server.sin_family = AF_INET;         // IP addresses.
    server.sin_addr.s_addr = INADDR_ANY; // Bind to all hosts.
    server.sin_port = htons(port);       // Listen on given port.

    // Bind our socket.
    if (bind(s, (const struct sockaddr *) &server, sizeof(server)) == -1) {
        fprintf(stderr, "socket: could not bind [%s]\n", strerror(errno));
        exit(1);
    }

    // Tell the kernel that we're listening on this socket.
    if (listen(s, 32) == -1) {
        fprintf(stderr, "socket: error on listen [%s]\n", strerror(errno));
        exit(1);
    }

    // Return the socket file descriptor to the caller.
    return s;
}

// Listens for incoming connections on a given server socket.
void server_listen(int s) {
    // Accept connections.
    for (;;) {
        // Define a structure for the client information.
        struct sockaddr_in cinfo;
        socklen_t client_addrlen = sizeof(cinfo);

        // Wait for and accept a single incoming connection.
        int c = accept(s, (struct sockaddr *) &cinfo, &client_addrlen);
        if (c == -1) {
            fprintf(stderr, "socket: error on accept [%s]\n", strerror(errno));
            exit(1);
        }

        // Formulate this client into a structure.
        // This is dynamically allocated to prevent a race condition.
        struct conn * client = (struct conn *) malloc(sizeof(struct conn));
        client->sock = c;
        client->info = cinfo;

        // Start a thread to handle this client.
        pthread_t thread;
        pthread_create(&thread, NULL, client_handle, (void *) client);
        pthread_detach(thread);
    }
}

// Takes a given client information and socket, and handles it.
void * client_handle(void * clientptr) {
    // Convert the arbitrary pointer to the actual client structure.
    struct conn * client = (struct conn *) clientptr;

    // Get some basic connection information.
    char * client_addr = inet_ntoa(client->info.sin_addr);
    int client_port = ntohs(client->info.sin_port);
    printf("Received a connection from %s on port %d.\n", client_addr, client_port);

    // Send the client a greeting.
    const char * message = "Oi, don't touch me.\n";
    send_bytes(client, message, strlen(message));

    // Close the connection.
    close(client->sock);

    // Free the related memory.
    free(client);

    // Must return a value.
    return NULL;
}

/* Helper auxiliary functions to aid in repetitive tasks. */

// send_bytes writes some given data to a file descriptor.
// It will keep writing until all bytes have been written.

void send_bytes(struct conn * client, const char * message, size_t length) {
    size_t sent = 0;
    while (sent != length) {
        // Write to socket and save the number of bytes written
        ssize_t n = write(client->sock, &message[sent], length-sent);
        if (n < 0) {
            printf("error: could not write to socket [%s]\n", strerror(errno));
        }

        // Add the sent bytes to the counter.
        sent += n;
    }
}
