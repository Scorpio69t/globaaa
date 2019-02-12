#ifndef MM_H_
#define MM_H_

#include <stdio.h>

int mm_init(void);
void *mm_malloc(size_t size);
void mm_free(void *ptr);
void *mm_realloc(void *ptr, size_t size);

#define ALIGNMENT 8
#define WORD_SIZE (sizeof(size_t))

#define TAGS_SIZE (2 * WORD_SIZE)

#define MINBLOCKSIZE (4 * WORD_SIZE)

#define ENDPOINT 9
#define ALLOCATED_FLAG 1
#define FREE_FLAG 0

typedef struct block {
    size_t size;
    // size is assumed to be a multiple of 8. The least-significant bit is
    // overloaded:
    //     if 0 the block is free
    //     if 1 the block is allocated
    size_t payload[];
    // the actual size of payload is given in the size field
    // for free blocks:
    //     payload[0] is the pointer to next free block;
    //     payload[1] is the pointer to the previous free block
    // there is a copy of the size field at the end of the block
} block_t;

void* check_flist(size_t size);
void print_heap_info(block_t* first, block_t* top);
int check_free_list();
int check_free_heap(block_t* block, block_t* top);
int find_in_list(block_t* block);

#endif  // MM_H_
