#include "mem.h"
#include "mstdio.h"
#include "debug.h"

extern uint64_t __END;

page *all_pages;
page_list *all_free_pages;

void zero_memory(void *start_address, int bytes){
    println("Zeroing Memory");
    print("Start address: "); printhex(start_address); println("");
    uint16_t *address = start_address;
    while(bytes--){
        *address++ = 0;
    }
    print("End address: "); printhex(address); println("");
    println("Memory Zeroed\n");
}

void init_memory(){
    uint64_t memory_size = get_memory_size();                       //128MB
    uint64_t number_of_pages = (memory_size)/(PAGE_SIZE);           //32768 4KB pages 
    uint64_t page_array_length = sizeof(page) * number_of_pages;    

    print("Memory size: ");
    printhex(memory_size);
    println("MB");

    print("Number of pages: ");
    printdec(number_of_pages);
    println("");

    print("Page array length: ");
    printhex(page_array_length);
    println(" bytes");

    all_pages = (page*)(&__END);
    print("Pages Metadata begin address: ");
    printhex(all_pages);
    println("");

    zero_memory(all_pages, page_array_length);

    print_memory();

    init_page_list(all_free_pages);
    println("initialized free page list");
    uint64_t kernel_pages = (uint64_t)(&__END)/(PAGE_SIZE);

 
    uint64_t i;
    for(i = 0; i < kernel_pages; i++){
        all_pages[i].allocated = 1;
        all_pages[i].kernel_reserved = 1;
        all_pages[i].virtual_mapped_address = i * PAGE_SIZE;

        //print("Marked kernel page "); printdec(i); print(" of 129\n");
    }
    println("Reserved pages");

    for(; i < 16380; i++){
        all_pages[i].allocated = 0;
        all_free_pages->append(all_free_pages, &all_pages[i]);
        //print("Marked page "); printdec(i); print(" of "); printdec(number_of_pages - kernel_pages); println("");
    }
    println("Memory Initialized"); 

}

uint64_t get_memory_size(){
    return 1024 * 1024 * 128;
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