#include "mm.h"
#include "string.h"

extern uint64_t __heap_start;
extern uint64_t __heap_end;

static uint8_t page_map[MAX_PAGES];
static uint64_t heap_start;
static uint64_t heap_end;
static uint32_t total_pages;

typedef struct block_header {
    uint32_t size;
    uint32_t magic;
    bool free;
    struct block_header *next;
} block_header_t;

#define BLOCK_MAGIC 0x4C415245

static block_header_t *free_list = NULL;
static uint64_t kmalloc_base = 0;
static uint64_t kmalloc_end = 0;

void mm_init(void) {
    heap_start = (uint64_t)&__heap_start;
    heap_end = (uint64_t)&__heap_end;

    total_pages = (heap_end - heap_start) / PAGE_SIZE;
    if (total_pages > MAX_PAGES) total_pages = MAX_PAGES;

    memset(page_map, MEM_FREE, total_pages);

    uint32_t kmalloc_pages = total_pages / 4;
    for (uint32_t i = 0; i < kmalloc_pages; i++) {
        page_map[i] = MEM_KERNEL;
    }

    kmalloc_base = heap_start;
    kmalloc_end = heap_start + kmalloc_pages * PAGE_SIZE;

    free_list = (block_header_t*)kmalloc_base;
    free_list->size = kmalloc_end - kmalloc_base - sizeof(block_header_t);
    free_list->magic = BLOCK_MAGIC;
    free_list->free = true;
    free_list->next = NULL;
}

void *kmalloc(size_t size) {
    size = ALIGN(size, 16);
    block_header_t *curr = free_list;

    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(block_header_t) + 16) {
                block_header_t *new_block = (block_header_t*)((uint8_t*)curr + sizeof(block_header_t) + size);
                new_block->size = curr->size - size - sizeof(block_header_t);
                new_block->magic = BLOCK_MAGIC;
                new_block->free = true;
                new_block->next = curr->next;
                curr->next = new_block;
                curr->size = size;
            }
            curr->free = false;
            return (void*)((uint8_t*)curr + sizeof(block_header_t));
        }
        curr = curr->next;
    }
    return NULL;
}

void *kcalloc(size_t count, size_t size) {
    void *ptr = kmalloc(count * size);
    if (ptr) memset(ptr, 0, count * size);
    return ptr;
}

void kfree(void *ptr) {
    if (!ptr) return;

    block_header_t *block = (block_header_t*)((uint8_t*)ptr - sizeof(block_header_t));
    if (block->magic != BLOCK_MAGIC) return;

    block->free = true;

    block_header_t *curr = free_list;
    while (curr) {
        if (curr->free && curr->next && curr->next->free) {
            curr->size += sizeof(block_header_t) + curr->next->size;
            curr->next = curr->next->next;
            continue;
        }
        curr = curr->next;
    }
}

void *page_alloc(uint32_t count) {
    uint32_t start = total_pages / 4;
    uint32_t found = 0;
    uint32_t first = 0;

    for (uint32_t i = start; i < total_pages; i++) {
        if (page_map[i] == MEM_FREE) {
            if (found == 0) first = i;
            found++;
            if (found == count) {
                for (uint32_t j = first; j < first + count; j++) {
                    page_map[j] = MEM_USED;
                }
                return (void*)(heap_start + first * PAGE_SIZE);
            }
        } else {
            found = 0;
        }
    }
    return NULL;
}

void page_free(void *ptr, uint32_t count) {
    uint64_t addr = (uint64_t)ptr;
    uint32_t first = (addr - heap_start) / PAGE_SIZE;

    for (uint32_t i = first; i < first + count && i < total_pages; i++) {
        page_map[i] = MEM_FREE;
    }
}

mem_info_t mm_get_info(void) {
    mem_info_t info;
    info.pages_total = total_pages;
    info.pages_used = 0;

    for (uint32_t i = 0; i < total_pages; i++) {
        if (page_map[i] != MEM_FREE) info.pages_used++;
    }

    info.total = (uint64_t)total_pages * PAGE_SIZE;
    info.used = (uint64_t)info.pages_used * PAGE_SIZE;
    info.free = info.total - info.used;

    return info;
}
