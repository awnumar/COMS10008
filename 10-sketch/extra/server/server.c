#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Declare global client structure.
struct conn {
    int sock;                // File descriptor for the socket.
    struct sockaddr_in info; // Struct that holds client information.
};

// Create a list for storing references to all active clients.
struct cl_struct {
    pthread_mutex_t lock; // Mutex lock.

    int len; // Number of active clients, i.e. size of the list.
    int cap; // Capacity of the list. Used to realloc when full.

    struct conn ** start; // Pointer to the start of the list, i.e. list[0].
};
struct cl_struct * clients;

// Declare function signatures.
int create_server(int port);
void server_listen(int s);
void * client_handle(void * clientptr);
int broadcast(struct conn * sender, unsigned char * message, size_t length);
int send_bytes(struct conn * client, unsigned char * message, size_t length);
unsigned char * recv_bytes(struct conn * client, size_t * len);
void add_client(struct conn * client);
void del_client(struct conn * client);
void attainLock(); void releaseLock();

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

    // Initialise the list.
    clients = (struct cl_struct *) malloc(sizeof(struct cl_struct));
    clients->start = (struct conn **) malloc(2 * sizeof(struct conn *));
    clients->len = 0; clients->cap = 2;

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

    // Allow port reuse.
    int F = 0;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &F, sizeof(int)) == -1) {
        fprintf(stderr, "socket: could not configure [%s]\n", strerror(errno));
        exit(1);
    }

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

        // Add this client to the global list.
        add_client(client);

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

    for (;;) {
        // Grab a message from the client. This is a blocking call.
        size_t len; unsigned char * message = recv_bytes(client, &len);
        if (len == 0) {
            printf("Disconnected from %s:%d.\n", client_addr, client_port);
            break;
        }

        if (len == 1 && message[0] == 0x00) {
            printf("Disconnected from %s:%d.\n", client_addr, client_port);
            break;
        }

        // Broadcast this message to everyone.
        int err = broadcast(client, message, len);
        if (err == -1) {
            printf("Disconnected from %s:%d.\n", client_addr, client_port);
            break;
        }
    }

    // Close the connection.
    close(client->sock);

    // Forget the client.
    del_client(client);

    // Free the related memory.
    free(client);

    // Must return a value.
    return NULL;
}

// Sends a given message to every client in the list of active clients, apart
// from to the original sender of the message. To send to everyone, set sender
// to NULL.
int broadcast(struct conn * sender, unsigned char * message, size_t length) {
    for (int i = 0; i < clients->len; i++) {
        if (clients->start[i] != sender) {
            int err = send_bytes(clients->start[i], message, length);
            if (err == -1) {
                return err;
            }
        }
    }

    // Everything went fine.
    return 0;
}

/* Helper auxiliary functions to aid in repetitive tasks. */

// send_bytes writes some given data to a file descriptor.
int send_bytes(struct conn * client, unsigned char * message, size_t length) {
    // Check the length is in range.
    assert(length > 0 && length < 4294967296);

    // Encode the length into raw binary.
    uint32_t rel_len = length;
    unsigned char len_bytes[4];
    len_bytes[0] = (rel_len >> 24) & 0xFF;
    len_bytes[1] = (rel_len >> 16) & 0xFF;
    len_bytes[2] = (rel_len >> 8) & 0xFF;
    len_bytes[3] = rel_len & 0xFF;

    // Send the length down the pipe.
    size_t sent = 0; while (sent != 4) {
        // Write to socket and save the number of bytes written.
        ssize_t n = write(client->sock, &len_bytes[sent], 4-sent);
        if (n < 0) {
            printf("error: could not write to socket [%s]\n", strerror(errno));
            return -1;
        }

        // Add the sent bytes to the counter.
        sent += n;
    }

    // Now send the actual data.
    sent = 0; while (sent != length) {
        // Write to socket and save the number of bytes written.
        ssize_t n = write(client->sock, &message[sent], length-sent);
        if (n < 0) {
            printf("error: could not write to socket [%s]\n", strerror(errno));
            return -1;
        }

        // Add the sent bytes to the counter.
        sent += n;
    }

    // Everything went well.
    return 0;
}

// recv_bytes takes a client structure and reads a message from it.
unsigned char * recv_bytes(struct conn * client, size_t * len) {
    // Grab the length first.
    unsigned char len_bytes[4];
    size_t received = 0; while (received != 4) {
        // Read from the socket and save the number of bytes written.
        ssize_t n = read(client->sock, &len_bytes[received], 4-received);
        if (n < 0) {
            printf("error: could not read from socket [%s]\n", strerror(errno));
            *len = 0; return NULL;
        }

        received += n;
    }

    // Convert this length to a workable datatype.
    uint32_t length = 0;
    length |= ((uint32_t) len_bytes[0]) << 24;
    length |= ((uint32_t) len_bytes[1]) << 16;
    length |= ((uint32_t) len_bytes[2]) << 8;
    length |= (uint32_t) len_bytes[3];

    // Create a buffer for storing this data.
    unsigned char * message = (unsigned char *) malloc(length);

    // Grab the data.
    received = 0; while (received != length) {
        // Read from the socket and save the number of bytes written.
        ssize_t n = read(client->sock, &message[received], length-received);
        if (n < 0) {
            printf("error: could not read from socket [%s]\n", strerror(errno));
            *len = 0; return NULL;
        }

        received += n;
    }

    // Return back to caller.
    *len = length;  // Set the length of the data.
    return message; // Return pointer to start of data.
}

// add_client adds a client to the global list of acive clients.
void add_client(struct conn * client) {
    // Attain a mutex lock on the list.
    attainLock();

    // Grow the array if we don't have enough space.
    if (clients->len + 1 > clients->cap) {
        clients->cap = (clients->cap + 1) * 2;
        clients->start = (struct conn **) realloc((void *) clients->start, clients->cap * sizeof(struct conn *));
    }

    // Now that we're guaranteed enough space, append the client.
    clients->start[clients->len] = client; clients->len++;

    // Release the mutex lock that we attained.
    releaseLock();
}

// del_client removes a client from the global list of active clients.
// del_client does not reallocate memory to shrink the array.
void del_client(struct conn * client) {
    // Attain a mutex lock on the list.
    attainLock();

    // Iterate through all of the clients...
    for (int i = 0; i < clients->len; i++) {
        // If current position matches argument...
        if (clients->start[i] == client) {
            // Move everything down to overwrite the element.
            for (int j = i; j < clients->len - 1; j++) {
                clients->start[j] = clients->start[j + 1];
            }

            // Update the length with the new value.
            clients->len--;

            // We don't need to continue searching,
            break;
        }
    }

    // Release the mutex lock that we attained.
    releaseLock();
}

// Attain a mutex lock on the list.
void attainLock() {
    if (pthread_mutex_lock(&clients->lock) != 0) {
        printf("lock: failed to lock mutex");
        exit(1);
    }
}

// Release the mutex lock on the list.
void releaseLock() {
    if (pthread_mutex_unlock(&clients->lock) != 0) {
        printf("lock: failed to unlock mutex");
        exit(1);
    }
}
