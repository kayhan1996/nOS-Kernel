#include "mem.h"


extern uint64_t __END;

page *all_pages;
struct page_list all_free_pages;

void init_memory(){
    uint64_t memory_size = get_memory_size();
    uint64_t number_of_pages = (memory_size)/(PAGE_SIZE);
    uint64_t page_array_length = sizeof(page) * number_of_pages;

    all_pages = (page*)(&__END);

    zero_memory(all_pages, page_array_length);
    init_page_list(&all_free_pages);

    uint64_t kernel_pages = (uint64_t)(&__END)/(PAGE_SIZE);

    int i;
    for(i = 0; i < kernel_pages; i++){
        all_pages[i].allocated = 1;
        all_pages[i].kernel_reserved = 1;
        all_pages[i].virtual_mapped_address = i * PAGE_SIZE;
    }

    for(; i < number_of_pages; i++){
        all_pages[i].allocated = 0;
    }

    
}

uint64_t get_memory_size(){
    return 1024 * 1024 * 256;
}

void zero_memory(void *start, uint64_t bytes){
    uint8_t * byte = start;
    while(bytes--){
        *byte++ = 0;
    }
}

void init_page_list(page_list *page_list){
    page_list->head = NULL;
    page_list->tail = NULL;
    page_list->size = 0;
    page_list->append = page_list_append;
    page_list->push = page_list_push;
    page_list->pop = page_list_pop;
}

void page_list_append(page_list *self, page *node){
    self->tail->next_page = node;
    node->prev_page = self->tail;
    node->next_page = NULL;
    self->tail = node;

    self->size += 1;

    if(self->head == NULL){
        self->head = node;
    }
}

void page_list_push(page_list *self, page *node){
    node->next_page = self->head;
    node->prev_page = NULL;
    if(self->head != NULL){
        self->head->prev_page = node;
    }
    self->head = node;
    if(self->tail = NULL){
        self->tail = node;
    }
}


page* page_list_pop(page_list *self)
{
    page *tmp = self->head;
    
    self->head = tmp->next_page;
    self->head->prev_page = NULL;
    tmp->prev_page = NULL;

    if(self->head == NULL){
        self->tail = NULL;
    }

    self->size--;

    return tmp;
}