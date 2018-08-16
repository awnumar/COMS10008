0x08: Pointers
==============

- Core program

        gcc -std=c99 -Wall -O3 -pedantic ./list.c -o ./list

    The way that the list has been implemented is to have an end-terminator
    node, which allows us to point "past" the last element. The terminator acts
    just like a normal node, pointing to the previous node, but its value is
    undefined.

    The list structure holds a variable pointer to the current first element,
    an unchanging pointer to the terminator node, and a variable pointer that
    represents the "cursor". The nodes hold two pointers to their respective
    neighbours, and also a single integer value.

- Extra credit program

        gcc -std=c99 -Wall -O3 -pedantic ./extra.c -o ./extra

    For the extra-credit portion of this week, I decided to modify this program
    to make it thread-safe, and able to handle variable data-types.

    Thread-safety was a case of creating a mutex for every list instance, and
    forcing each API function to attain a lock before reading or writing. I
    acknowledge the potential performance considerations that must be taken,
    but this is the most simple solution that does not sacrifice a lot. Also,
    note that accessing a list without the API functions is undefined, and so a
    potential user must take care to acquire the exposed lock themselves.

    The handling of variable data-types is a slightly more interesting problem.
    I decided to solve it by patching the item type as such:

        typedef int item; ==> typedef void * item;

    This way, the nodes no longer hold an actual value, but rather hold
    arbitrary pointers to anything. The user must dynamically allocate some
    memory, and also cast the void pointer to the appropriate data type when
    referencing.

    I also decided to implement push() and pop() functions, which are described
    here: https://en.wikipedia.org/wiki/Stack_(abstract_data_type). They have
    the following type signatures:

        void push(list * l, item x);
        item pop(list * l);
