0x0A - Extra Credit Work
========================

For the extra credit part of the assignment this week, I decided to create the
backbones of a simple group-chat application.

Server
``````

    The server-side is setup as follows.

    First we create and bind to a TCP socket and wait for incoming connections.
    Upon receiving such a connection, we wrap it up in a struct and store it in
    a global array of active clients. This global array is protected by a mutex
    to prevent race-conditions.

    The global array is also dynamically-sized. It is a structure that holds a
    mutex, two metadata fields that hold the length and capacity of the array,
    and a final pointer to the start of the array's memory. It is created and
    initialised with a length of zero (representing the number of active
    clients), and a capacity of two (representing the size of the dynamically
    allocated memory). When adding a new client, if it does not fit, the memory
    is reallocated by incrementing the current capacity and multiplying the
    result by two.

    Then, we create a detached thread for this client and wait for data. Any
    data that is received is broadcast to every other connected client.

    The method of transporting data is relatively simple but expandable. First,
    we encode the length of the data that we wish to send in a fixed-size, four-
    byte array. Then we write these four bytes to the file descriptor and follow
    them up with the data itself. On the receiving end, the client simply reads
    exactly four bytes from the socket, decodes it into a length, and then reads
    exactly that amount of data more. This way we have the reliability of the
    TCP protocol with a UDP-style message system.

Client
``````

    The client-side is setup as follows.

    We initialise the environment by making stdout unbuffered, and then we
    follow that up with connecting to a given server. After that, we create a
    thread that listens for incoming data on the socket and prints it to the
    screen.

    After that, we simply hand over control to the client by displaying a
    prompt, receiving input on stdin, and sending it to the server. This is
    currently implemented with the very basic POSIX 200809L getline function,
    but obviously in an improved version you would use something like getch for
    its precise control and better handling of memory.

    When the client wishes to exit, it send a single null byte to the server,
    and the server then shuts down the socket, frees the memory, and forgets
    about the client.
