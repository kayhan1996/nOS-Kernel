#ifndef __LINKED_LIST_H__
#define __LINKED_LIST_H__

#include <Libraries/printx.h>
#include <stddef.h>
#include <stdint.h>

/* Define Structures */

typedef struct LinkedList LinkedList;
typedef struct LinkNode LinkNode;

struct LinkNode{
    struct LinkNode *next;
    struct LinkNode *prev;
};

struct LinkedList{
    struct LinkNode *head;
    struct LinkNode *tail;
    int count;

    struct {
    /* Adds a node to the front of the list */
    void (*add)(LinkedList *list, LinkNode *node);
    /* Adds a node to the end of the list */
    void (*add_end)(LinkedList *list, LinkNode *node);

    /* Removes a node from the list */
    void (*remove_node)(LinkedList *list, LinkNode *node);

    /* Removes a node from the front of the list */
    LinkNode* (*remove)(LinkedList *list);
    /* Removes a node from the end of the list */
    LinkNode* (*remove_end)(LinkedList *list);

    /* Retrieves the number of elements in the list */
    int (*size)(LinkedList *list);
    
    };
};

/* Define Function Prototypes */
/* Initialize a linked list */
void init_linked_list(LinkedList *list);

/* Finds the payload associated with the link node */
#define payload(type, n)({                             \
    ((type*)((uint64_t)n - offsetof(type, node)));     \
})

#endif /* __LINKED_LIST_H__ */
