#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>

// Declare global variables.
int s; // Server socket.

// Declare function signatures.
void create_conn(char * address, int port);
void interact(); void * recv_messages(void * nil);
void send_bytes(unsigned char * message, size_t length);
unsigned char * recv_bytes(size_t * len);

int main(int argc, char * argv[]) {
    // Print usage information.
    if (argc != 3) {
        fprintf(stderr, "Usage: ./client [address] [port]\n");
        return 1;
    }

    // Convert the port to a usable datatype.
    int port = strtol(argv[2], NULL, 10);
    if (port == 0) {
        fprintf(stderr, "port invalid\n");
        exit(1);
    }

    // Make stdout unbuffered.
    setbuf(stdout, NULL);

    // Connect to the server.
    create_conn(argv[1], port);

    // Hand over interaction to user.
    interact();

    // Tell the server that we're exiting.
    unsigned char nil = 0x00;
    send_bytes(&nil, 1);

    // Close the socket.
    close(s);
}

// Connect to the server and return the file descriptor.
void create_conn(char * address, int port) {
    // Create the socket.
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == -1) {
        fprintf(stderr, "socket: creation failed [%s]\n", strerror(errno));
        exit(1);
    }

    // Create and configure the server structure.
    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(address);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Connect to the server.
    if (connect(s, (struct sockaddr *) &server, sizeof(server)) < 0) {
        fprintf(stderr, "socket: could not connect [%s]\n", strerror(errno));
        exit(1);
    }
}

void * recv_messages(void * nil) {
    for (;;) {
        // Grab a message and its length. This is a blocking call.
        size_t len = 0; unsigned char * message = recv_bytes(&len);

        // Print it to the screen.
        printf("\r[Anonymous] %s[Me] ", message);
    }
}

// Interact with the server.
void interact() {
    // Print a help message.
    printf("Connected. Use 'q' to quit.\n");

    // Start a handler to print incoming messages.
    pthread_t thread;
    pthread_create(&thread, NULL, recv_messages, NULL);
    pthread_detach(thread);

    // Allow the user to interact.
    for (;;) {
        // Print the prompt.
        printf("[Me] ");

        // Create a buffer for storing the input, and the length of the input.
        size_t n = 4096; char * buf = (char *) malloc(n);

        // Get a single line from standard input.
        ssize_t read = getline(&buf, &n, stdin);
        if (read == -1) {
            // Probably an EOF. Alternative is a malloc err, which is unlikely.
            return;
        }

        // Did the user want to exit?
        if (strcmp(buf, "q\n") == 0) {
            return;
        }

        // Send to the server.
        send_bytes((unsigned char *) buf, read);
    }
}

/* Helper auxiliary functions to aid in repetitive tasks. */

// send_bytes sends some given data to the server.
void send_bytes(unsigned char * message, size_t length) {
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
        ssize_t n = write(s, &len_bytes[sent], 4-sent);
        if (n < 0) {
            printf("error: could not write to socket [%s]\n", strerror(errno));
            exit(1);
        }

        // Add the sent bytes to the counter.
        sent += n;
    }

    // Now send the actual data.
    sent = 0; while (sent != length) {
        // Write to socket and save the number of bytes written.
        ssize_t n = write(s, &message[sent], length-sent);
        if (n < 0) {
            printf("error: could not write to socket [%s]\n", strerror(errno));
            exit(1);
        }

        // Add the sent bytes to the counter.
        sent += n;
    }
}

// recv_bytes takes a client structure and reads a message from it.
unsigned char * recv_bytes(size_t * len) {
    // Grab the length first.
    unsigned char len_bytes[4];
    size_t received = 0; while (received != 4) {
        // Read from the socket and save the number of bytes written.
        ssize_t n = read(s, &len_bytes[received], 4-received);
        if (n < 0) {
            printf("error: could not read from socket [%s]\n", strerror(errno));
            exit(1);
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
        ssize_t n = read(s, &message[received], length-received);
        if (n < 0) {
            printf("error: could not read from socket [%s]\n", strerror(errno));
            exit(1);
        }

        received += n;
    }

    // Return back to caller.
    *len = length;  // Set the length of the data.
    return message; // Return pointer to start of data.
}
