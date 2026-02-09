#ifndef MM_H
#define MM_H

#include "lareos.h"

#define PAGE_SIZE       4096
#define MAX_PAGES       16384

#define MEM_FREE        0
#define MEM_USED        1
#define MEM_KERNEL      2

typedef struct {
    uint64_t total;
    uint64_t used;
    uint64_t free;
    uint32_t pages_total;
    uint32_t pages_used;
} mem_info_t;

void mm_init(void);
void *kmalloc(size_t size);
void *kcalloc(size_t count, size_t size);
void kfree(void *ptr);
void *page_alloc(uint32_t count);
void page_free(void *ptr, uint32_t count);
mem_info_t mm_get_info(void);

#endif
