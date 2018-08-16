/*
 * Week 0x08 - Pointers
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

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
    node * curr_ptr; // Current node.
    node * frst_ptr; // First node.
    node * term_ptr; // Static pointer to last, "terminator", node.
};

list * newList() {
    // Dynamically allocate an instance of the list structure on the heap.
    list * l = (list *) malloc(sizeof(list));

    // Create and initialise the final, "terminator" node.
    node * term = (node *) malloc(sizeof(node));
    term->next_ptr = NULL; term->prev_ptr = NULL;

    // Initialise the list's pointers appropriately.
    l->curr_ptr = term; l->frst_ptr = term; l->term_ptr = term;

    // Return the list instance.
    return l;
}

void start(list *l) {
    l->curr_ptr = l->frst_ptr;
}

void end(list *l) {
    l->curr_ptr = l->term_ptr;
}

bool atStart(list *l) {
    return l->curr_ptr == l->frst_ptr;
}

bool atEnd(list *l) {
    return l->curr_ptr == l->term_ptr;
}

void forward(list *l) {
    // Panic if we're already at the end.
    assert(!atEnd(l));

    // Move forward an element. Stagger it to avoid undefined behaviour.
    node * next_ptr = l->curr_ptr->next_ptr; l->curr_ptr = next_ptr;
}

void backward(list *l) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    // Move back an element. Stagger it to avoid undefined behaviour.
    node * prev_ptr = l->curr_ptr->prev_ptr; l->curr_ptr = prev_ptr;
}

void insertBefore(list *l, item x) {
    // Dynamically allocate a node, and initialise it.
    node * n = (node *) malloc(sizeof(node)); n->value = x;

    // Hold a pointer to the previous node.
    node * pr = l->curr_ptr->prev_ptr;
    if (pr == NULL) {
        l->frst_ptr = n;
    } else {
        pr->next_ptr = n;
    }

    // Rearrange the pointers to insert n into the list.
    n->prev_ptr = pr; n->next_ptr = l->curr_ptr; l->curr_ptr->prev_ptr = n;
}

void insertAfter(list *l, item x) {
    insertBefore(l, x); backward(l);
}

item getBefore(list *l) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    return l->curr_ptr->prev_ptr->value;
}

item getAfter(list *l) {
    // Panic if we're already at the end.
    assert(!atEnd(l));

    return l->curr_ptr->value;
}

void setBefore(list *l, item x) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    l->curr_ptr->prev_ptr->value = x;
}

void setAfter(list *l, item x) {
    // Panic if we're already at the end.
    assert(!atEnd(l));

    l->curr_ptr->value = x;
}

void deleteBefore(list *l) {
    // Panic if we're already at the start.
    assert(!atStart(l));

    // Set the first pointer appropriately.
    if (l->curr_ptr->prev_ptr == l->frst_ptr) {
        l->frst_ptr = l->curr_ptr;
    }

    // Hold a pointer to the node that we are removing.
    node * toDel = l->curr_ptr->prev_ptr;

    // Rearrange the pointers so as to "skip" the previous node.
    node * prr = l->curr_ptr->prev_ptr->prev_ptr; l->curr_ptr->prev_ptr = prr;

    // Deallocate the node.
    free(toDel);
}

void deleteAfter(list *l) {
    forward(l); deleteBefore(l);
}

/******************************************************************************/

void test() {
    // Test newList.
    list * l = newList();
    assert(l != NULL);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->frst_ptr == l->term_ptr);
    assert(l->term_ptr->next_ptr == NULL);
    assert(l->term_ptr->prev_ptr == NULL);

    // Test insertBefore and insertAfter.
    insertBefore(l, 42);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->curr_ptr->prev_ptr->value == 42);
    assert(l->curr_ptr->prev_ptr->prev_ptr == NULL);
    assert(l->frst_ptr == l->curr_ptr->prev_ptr);
    assert(l->frst_ptr->value == 42);
    assert(l->frst_ptr->next_ptr == l->term_ptr);
    insertAfter(l, 69);
    assert(l->curr_ptr->value == 69);
    assert(l->frst_ptr->next_ptr == l->curr_ptr);
    assert(l->term_ptr->prev_ptr == l->curr_ptr);
    assert(l->curr_ptr->next_ptr == l->term_ptr);
    assert(l->curr_ptr->prev_ptr == l->frst_ptr);
    assert(l->frst_ptr->value == 42);
    assert(l->frst_ptr->next_ptr->value == 69);
    assert(l->frst_ptr->next_ptr->next_ptr == l->term_ptr);
    assert(l->term_ptr->prev_ptr->value == 69);
    assert(l->term_ptr->prev_ptr->prev_ptr->value == 42);
    assert(l->term_ptr->prev_ptr->prev_ptr->prev_ptr == NULL);

    // Test start and end.
    start(l);
    assert(l->curr_ptr->value == 42);
    assert(l->curr_ptr == l->frst_ptr);
    assert(l->curr_ptr->prev_ptr == NULL);
    end(l);
    assert(l->curr_ptr == l->term_ptr);

    // Test atStart and atEnd.
    start(l); assert(atStart(l));
    end(l); assert(atEnd(l));

    // Test forward and backward.
    start(l);
    forward(l); assert(l->curr_ptr->value == 69);
    backward(l); assert(l->curr_ptr->value == 42);
    assert(atStart(l));

    // Test getBefore and getAfter.
    forward(l); assert(getBefore(l) == 42);
    assert(getAfter(l) == 69);

    // Test setBefore and setAfter.
    setBefore(l, 69); assert(getBefore(l) == 69);
    setAfter(l, 42); assert(getAfter(l) == 42);

    // Test deleteBefore and deleteAfter.
    deleteBefore(l); assert(l->curr_ptr->prev_ptr == NULL);
    deleteAfter(l); assert(l->curr_ptr == l->term_ptr);
    assert(l->curr_ptr == l->term_ptr);
    assert(l->frst_ptr == l->term_ptr);
    assert(l->term_ptr->prev_ptr == NULL);

    // Destroy the list instance to prevent a memory leak.
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
