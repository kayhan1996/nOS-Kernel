#include "mem.h"
#include "mstdio.h"
#include "debug.h"

extern uint64_t __end;
extern uint64_t __start;
extern uint64_t __text_start;
extern uint64_t __rodata_start;
extern uint64_t __data_start;
extern uint64_t __bss_start;
extern uint64_t __bss_end;

page_t *all_pages;
page_list_t all_free_pages;

void zero_memory(void *start_address, int bytes){
    println("Zeroing Memory");
    print("Start address: "); printhex(start_address); println("");
    uint8_t *address = start_address;
    while(bytes--){
        *address++ = 0x0;
    }
    print("End address: "); printhex(address); println("");
    println("Memory Zeroed\n");
}

void init_memory(){
    uint64_t memory_size = get_memory_size();                       //128MB
    uint64_t number_of_pages = (memory_size)/(PAGE_SIZE);           //32768 4KB pages 
    uint64_t page_array_length = sizeof(page_t) * number_of_pages;    

    all_pages = (page_t *)(&__end);
    zero_memory(&__end, page_array_length);
    init_page_list(&all_free_pages);
    
    uint64_t i;

    uint64_t kernel_pages = (uint64_t)(&__end)/(PAGE_SIZE);

    print("Number of kernel pages: "); printhex(kernel_pages); println("");
    for(i = 0; i < kernel_pages; i++){
        all_pages[i].flags.allocated = 1;
        all_pages[i].flags.kernel_page = 1;
        all_pages[i].vmapped_address = i * PAGE_SIZE;
        print_memory(&all_pages[i].flags);
    }
    
    for(; i < number_of_pages; i++){
        all_pages[i].flags.allocated = 0;
        all_pages[i].vmapped_address = i * PAGE_SIZE;
        all_free_pages.append(&all_free_pages, &all_pages[i]);
    }
}

uint64_t get_memory_size(){
    return 1024 * 1024 * 128;
}

void inline init_page_list(page_list_t *page_list){
    page_list->head = NULL;
    page_list->tail = NULL;
    page_list->size = 0xfffff;
    page_list->append = page_list_append;
    page_list->push = page_list_push;
    page_list->pop = page_list_pop;
}

void page_list_append(page_list_t *self, page_t *node){
    self->tail->next_page = node;
    node->prev_page = self->tail;
    node->next_page = NULL;
    self->tail = node;

    self->size += 1;

    if(self->head == NULL){
        self->head = node;
    }
}

void page_list_push(page_list_t *self, page_t *node){
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


page_t* page_list_pop(page_list_t *self)
{
    page_t *tmp = self->head;
    
    self->head = tmp->next_page;
    self->head->prev_page = NULL;
    tmp->prev_page = NULL;

    if(self->head == NULL){
        self->tail = NULL;
    }

    self->size--;

    return tmp;
}