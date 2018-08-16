/*
 * Week 0x08 - Pointers (Extra Credit)
 */

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "extra.h"

// Define the structure of a node on the list.
typedef struct node node;
struct node {
    // Actual value of this node.
    item value;

    // Pointers to first and last element
    node * prev_ptr;
    node * next_ptr;
};

// Define the structure of a list instance.
struct list {
    pthread_mutex_t lock; // Mutex lock.

    node * curr_ptr; // Current node.
    node * frst_ptr; // First node.
    node * term_ptr; // Static pointer to last, "terminator", node.
};

void lockList(list * l) {
    if (pthread_mutex_lock(&l->lock) != 0) {
        printf("lock: failed to lock mutex");
        exit(1);
    }
}

void unlockList(list * l) {
    if (pthread_mutex_unlock(&l->lock) != 0) {
        printf("lock: failed to unlock mutex");
        exit(1);
    }
}

list * newList() {
    // Dynamically allocate an instance of the list structure on the heap.
    list * l = (list *) malloc(sizeof(list));

    // Create and initialise the final, "terminator" node.
    node * term = (node *) malloc(sizeof(node));
    term->next_ptr = NULL; term->prev_ptr = NULL;

    // Initialise the list's pointers appropriately.
    l->curr_ptr = term; l->frst_ptr = term; l->term_ptr = term;

    // Initialise this list instance's mutex.
    if (pthread_mutex_init(&l->lock, NULL) != 0) {
        printf("lock: failed to init mutex");
        exit(1);
    }

    // Return the list instance.
    return l;
}

void start(list * l) {
    lockList(l);
    l->curr_ptr = l->frst_ptr;
    unlockList(l);
}

void end(list * l) {
    lockList(l);
    l->curr_ptr = l->term_ptr;
    unlockList(l);
}

bool atStart(list * l) {
    lockList(l);
    return l->curr_ptr == l->frst_ptr;
    unlockList(l);
}

bool atEnd(list * l) {
    lockList(l);
    return l->curr_ptr == l->term_ptr;
    unlockList(l);
}

void forward(list * l) {
    // Panic if we're already at the end.
    assert(!atEnd(l));

    lockList(l);
    // Move forward an element. Stagger it to avoid undefined behaviour.
    node * next_ptr = l->curr_ptr->next_ptr; l->curr_ptr = next_ptr;
    unlockList(l);
}

void backward(list * l) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    lockList(l);
    // Move back an element. Stagger it to avoid undefined behaviour.
    node * prev_ptr = l->curr_ptr->prev_ptr; l->curr_ptr = prev_ptr;
    unlockList(l);
}

void insertBefore(list * l, item x) {
    // Dynamically allocate a node, and initialise it.
    node * n = (node *) malloc(sizeof(node)); n->value = x;

    lockList(l);
    // Hold a pointer to the previous node.
    node * pr = l->curr_ptr->prev_ptr;
    if (pr == NULL) {
        l->frst_ptr = n;
    } else {
        pr->next_ptr = n;
    }

    // Rearrange the pointers to insert n into the list.
    n->prev_ptr = pr; n->next_ptr = l->curr_ptr; l->curr_ptr->prev_ptr = n;
    unlockList(l);
}

void insertAfter(list * l, item x) {
    insertBefore(l, x); backward(l);
}

item getBefore(list * l) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    lockList(l);
    return l->curr_ptr->prev_ptr->value;
    unlockList(l);
}

item getAfter(list * l) {
    // Panic if we're already at the end.
    assert(!atEnd(l));

    lockList(l);
    return l->curr_ptr->value;
    unlockList(l);
}

void setBefore(list * l, item x) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    lockList(l);
    l->curr_ptr->prev_ptr->value = x;
    unlockList(l);
}

void setAfter(list * l, item x) {
    // Panic if we're already at the end.
    assert(!atEnd(l));

    lockList(l);
    l->curr_ptr->value = x;
    unlockList(l);
}

void deleteBefore(list * l) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    lockList(l);
    // Set the first pointer appropriately.
    if (l->curr_ptr->prev_ptr == l->frst_ptr) {
        l->frst_ptr = l->curr_ptr;
    }

    // Hold a pointer to the node that we are removing.
    node * toDel = l->curr_ptr->prev_ptr;

    // Rearrange the pointers so as to "skip" the previous node.
    node * prr = l->curr_ptr->prev_ptr->prev_ptr; l->curr_ptr->prev_ptr = prr;
    unlockList(l);

    // Deallocate the node.
    free(toDel);
}

void deleteAfter(list * l) {
    forward(l); deleteBefore(l);
}

void push(list * l, item x) {
    end(l);
    insertBefore(l, x);
}

item pop(list * l) {
    end(l);
    item i = getBefore(l);
    deleteBefore(l);
    return i;
}

/******************************************************************************/

void test() {
    // Example values that we will hold in the nodes.
    int * fourtyTwo = (int *) malloc(sizeof(int)); *fourtyTwo = 42;
    int * sixtyNine = (int *) malloc(sizeof(int)); *sixtyNine = 69;

    // Test newList.
    list * l = newList();
    assert(l != NULL);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->frst_ptr == l->term_ptr);
    assert(l->term_ptr->next_ptr == NULL);
    assert(l->term_ptr->prev_ptr == NULL);

    // Test insertBefore and insertAfter.
    insertBefore(l, (void *) fourtyTwo);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->curr_ptr->prev_ptr->value == (void *) fourtyTwo);
    assert(l->curr_ptr->prev_ptr->prev_ptr == NULL);
    assert(l->frst_ptr == l->curr_ptr->prev_ptr);
    assert(l->frst_ptr->value == (void *) fourtyTwo);
    assert(l->frst_ptr->next_ptr == l->term_ptr);
    insertAfter(l, (void *) sixtyNine);
    assert(l->curr_ptr->value == (void *) sixtyNine);
    assert(l->frst_ptr->next_ptr == l->curr_ptr);
    assert(l->term_ptr->prev_ptr == l->curr_ptr);
    assert(l->curr_ptr->next_ptr == l->term_ptr);
    assert(l->curr_ptr->prev_ptr == l->frst_ptr);
    assert(l->frst_ptr->value == (void *) fourtyTwo);
    assert(l->frst_ptr->next_ptr->value == (void *) sixtyNine);
    assert(l->frst_ptr->next_ptr->next_ptr == l->term_ptr);
    assert(l->term_ptr->prev_ptr->value == (void *) sixtyNine);
    assert(l->term_ptr->prev_ptr->prev_ptr->value == (void *) fourtyTwo);
    assert(l->term_ptr->prev_ptr->prev_ptr->prev_ptr == NULL);

    // Test start and end.
    start(l);
    assert(l->curr_ptr->value == (void *) fourtyTwo);
    assert(l->curr_ptr == l->frst_ptr);
    assert(l->curr_ptr->prev_ptr == NULL);
    end(l);
    assert(l->curr_ptr == l->term_ptr);

    // Test atStart and atEnd.
    start(l); assert(atStart(l));
    end(l); assert(atEnd(l));

    // Test forward and backward.
    start(l);
    forward(l); assert(l->curr_ptr->value == (void *) sixtyNine);
    backward(l); assert(l->curr_ptr->value == (void *) fourtyTwo);
    assert(atStart(l));

    // Test getBefore and getAfter.
    forward(l); assert(getBefore(l) == (void *) fourtyTwo);
    assert(getAfter(l) == (void *) sixtyNine);

    // Test setBefore and setAfter.
    setBefore(l, (void *) sixtyNine);
    assert(getBefore(l) == (void *) sixtyNine);
    setAfter(l, (void *) fourtyTwo);
    assert(getAfter(l) == (void *) fourtyTwo);

    // Test deleteBefore and deleteAfter.
    deleteBefore(l); assert(l->curr_ptr->prev_ptr == NULL);
    deleteAfter(l); assert(l->curr_ptr == l->term_ptr);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->frst_ptr == l->term_ptr);
    assert(l->term_ptr->prev_ptr == NULL);

    // Test push and pop.
    push(l, (void *) fourtyTwo);
    push(l, (void *) sixtyNine);
    assert(pop(l) == (void *) sixtyNine);
    assert(pop(l) == (void *) fourtyTwo);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->frst_ptr == l->term_ptr);
    assert(l->term_ptr->prev_ptr == NULL);

    // Destroy the list instance to prevent a memory leak.
    pthread_mutex_destroy(&l->lock);
    free(l->term_ptr);
    free(l);
}

int main(int argc, char * argv[]) {
    if (argc == 1) {
        // Run tests.
        test();

        // We're still here, so everything went well.
        printf("All tests passed.\n");
    }

    return 0;
}
