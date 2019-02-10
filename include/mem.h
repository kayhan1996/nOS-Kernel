#include "stdint.h"
#include "stddef.h"
#ifndef MEM_H
#define MEM_H

    #define PAGE_SIZE 4096
    void init_memory();

    uint64_t get_memory_size();

    typedef struct page{
        uint8_t allocated : 1;
        uint8_t kernel_reserved : 1;
        uint64_t reserved : 62;
        page *next_page : 64;
        page *prev_page : 64;
        uint64_t virtual_mapped_address : 64;
    } page;

    typedef struct page_list{
        struct page *head;
        struct page *tail;
        uint64_t size;

        void (*append)(struct page_list *self, page *node);
        void (*push)(struct page_list *self, page *node);
        page* (*pop)(struct page_list *self);

    } page_list;

    void init_page_list(page_list *page_list);

    void page_list_append(page_list *self, page *node);
    void page_list_push(page_list *self, page *node);
    page* page_list_pop(page_list *self);
    
    

#endif /*MEM_H*/