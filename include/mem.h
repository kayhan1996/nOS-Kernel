#include "stdint.h"
#include "stddef.h"
#ifndef MEM_H
#define MEM_H

    #define PAGE_SIZE 4096

    void init_memory();
    void zero_memory(void *start_address, int bytes);
    void* allocate_page();
    void traverse_list();

    uint64_t get_memory_size();

    typedef struct page_flags{
        uint8_t  allocated : 1;
        uint8_t  kernel_page : 1;
        uint64_t reserved    : 62;  //padding to make struct 64 bits 
    } page_flags_t;

    typedef struct page{
        uint64_t vmapped_address;
        page_flags_t flags;
        struct page *prev_page;
        struct page *next_page;
    } page_t;

    typedef struct page_list{
        page_t *head;
        page_t *tail;
        uint64_t size;

        void (*append)(struct page_list *self, page_t *node);
        void (*push)(struct page_list *self, page_t *node);
        page_t* (*pop)(struct page_list *self);

    } page_list_t;

    void init_page_list(page_list_t *page_list);

    void page_list_append(page_list_t *self, page_t *node);
    void page_list_push(page_list_t *self, page_t *node);
    page_t* page_list_pop(page_list_t *self);
    
    

#endif /*MEM_H*/