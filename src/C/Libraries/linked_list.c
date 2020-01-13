
#include "linked_list.h"

/* Functions */

/* Adds a node to the beginning of the list */
void add_ll(LinkedList *list, LinkNode *node){
    if(list->head == NULL){
        list->head = node;
        list->tail = node;
    }else{
        list->head->prev = node;
        node->next = list->head;
        node->prev = NULL;
        list->head = node;
    }
    list->count++;
}

/* Adds a node to the end of the list */
void add_end_ll(LinkedList *list, LinkNode *node){
    if(list->head == NULL){
        list->head = node;
        list->tail = node;
    }else{
        list->tail->next = node;
        node->prev = list->tail;
        node->next = NULL;
        list->tail = node;
    }
    list->count++;
}

/* Removes Node from the list */
void remove_node_ll(LinkedList *list, LinkNode *node){
    if(list->head == node){
        list->remove(list);
    }else if(list->tail == node){
        list->remove_end(list);
    }else{
        node->prev->next = node->next;
        node->next->prev = node->prev;
        list->count--;
    }
}

/* Removes a node from the beginning of the list */
LinkNode* remove_ll(LinkedList *list){
    LinkNode *tmp = list->head;
    list->head = tmp->next;
    if(list->head != NULL)
        list->head->prev = NULL;
    else
        list->tail = NULL;
    tmp->next = NULL;
    list->count--;
    return tmp;
}

/* Removes a node from the end of the list */
LinkNode* remove_end_ll(LinkedList *list){
    LinkNode *tmp = list->tail;
    list->tail = tmp->prev;
    if(list->tail != NULL)
        list->tail->next = NULL;
    else
        list->head = NULL;
    tmp->prev = NULL;
    list->count--;
    return tmp;
}

/* Returns the number of elements in the list */
int size_ll(LinkedList *list){
    return list->count;
}

/* Initializes a new linked list */
void init_linked_list(LinkedList *list){
    list->add = add_ll;
    list->add_end = add_end_ll;
    list->remove_node = remove_node_ll;
    list->remove = remove_ll;
    list->remove_end = remove_end_ll;
    list->size = size_ll;

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}