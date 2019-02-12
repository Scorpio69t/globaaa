#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "./mm.h"
#include "./memlib.h"
#include "./mminline.h"


// rounds up to the nearest multiple of WORD_SIZE
static inline size_t align(size_t size) {
    return (((size) + (WORD_SIZE - 1)) & ~(WORD_SIZE - 1));
}

int mm_check_heap(void);

/*
 *                             _       _ _
 *     _ __ ___  _ __ ___     (_)_ __ (_) |_
 *    | '_ ` _ \| '_ ` _ \    | | '_ \| | __|
 *    | | | | | | | | | | |   | | | | | | |_
 *    |_| |_| |_|_| |_| |_|___|_|_| |_|_|\__|
 *                       |_____|
 *
 * initializes the dynamic storage allocator (allocate initial heap space)
 * arguments: none
 * returns: 0, if successful
 *         -1, if an error occurs
 */
int mm_init(void) {

  // set the free list to NULL
  flist_first = NULL;

  // set up initial size; returns address of new region
  void* initial = mem_sbrk(TAGS_SIZE);

  // cast a block at the beginning of the heap as the prologue
  size_t* prologue_p = initial;

  // set initial value in prologue
  *prologue_p = ENDPOINT;

  // find epilogue
  size_t* epilogue_p = (size_t*) block_next((block_t*) prologue_p);

  // set size of the epilogue
  *epilogue_p = ENDPOINT;

  return 0;
}

/*     _ __ ___  _ __ ___      _ __ ___   __ _| | | ___   ___
 *    | '_ ` _ \| '_ ` _ \    | '_ ` _ \ / _` | | |/ _ \ / __|
 *    | | | | | | | | | | |   | | | | | | (_| | | | (_) | (__
 *    |_| |_| |_|_| |_| |_|___|_| |_| |_|\__,_|_|_|\___/ \___|
 *                       |_____|
 *
 * allocates a block of memory and returns a pointer to that block's payload
 * arguments: size: the desired payload size for the block
 * returns: a pointer to the newly-allocated block's payload (whose size
 *          is a multiple of ALIGNMENT), or NULL if an error occurred
 */
void *mm_malloc(size_t size) {

  // first check to see if there's a suitable block in the free list
  void* block = check_flist(size);

  if(block)
    return block;

  // if no free blocks in free list, allocate more memory, memory will be aligned
  size_t updated_size = align(size);
  block_t* updated = mem_sbrk(updated_size + WORD_SIZE);

  // update new block and override previous epilogue
  updated = block_prev(updated);
  block_set_size_and_allocated(updated, updated_size + TAGS_SIZE, ALLOCATED_FLAG);

  // new epilogue
  size_t* epilogue_p = mem_sbrk(WORD_SIZE);

  // set size of the epilogue
  *epilogue_p = ENDPOINT;

  return updated->payload;
}

/*
 * check the free list first for a block of suitable length
 */
void* check_flist(size_t size) {
  // if free list is not NULL
  if(flist_first) {

    // first block in the list
    block_t* initial = flist_first;

    // the aligned size of the block you're looking for
    size_t aligned = align(size);

    do {
      // find size of block
      size_t initial_size = block_size(initial);

      // if block from free list has enough space
      if(initial_size >= (aligned + TAGS_SIZE)) {
	// pull block from list
	pull_free_block(initial);

        /* if block you're pulling from free list has enough space to insert
         * part of it back into the free list. */
	if((initial_size - (aligned + TAGS_SIZE)) >= MINBLOCKSIZE) {

	  // set size and allocated
	  block_set_size_and_allocated(initial, aligned + TAGS_SIZE, ALLOCATED_FLAG);

	  // reinsert extra memory back into free list
	  size_t remainder = initial_size - (aligned + TAGS_SIZE);
	  block_t* reinsert = block_next(initial);
	  block_set_size_and_allocated(reinsert, remainder, FREE_FLAG);
	  insert_free_block(reinsert);
	}
	else
	  block_set_size_and_allocated(initial, initial_size, ALLOCATED_FLAG);

	return initial->payload;
      }
      else
        initial = block_next_free(initial);

    } while(initial != flist_first);
  }

  return NULL;
}

/*                              __
 *     _ __ ___  _ __ ___      / _|_ __ ___  ___
 *    | '_ ` _ \| '_ ` _ \    | |_| '__/ _ \/ _ \
 *    | | | | | | | | | | |   |  _| | |  __/  __/
 *    |_| |_| |_|_| |_| |_|___|_| |_|  \___|\___|
 *                       |_____|
 *
 * frees a block of memory, enabling it to be reused later
 * arguments: ptr: pointer to the allocated block to free
 * returns: nothing
 */
void mm_free(void *ptr) {

  // if ptr equals NULL
  if(!ptr)
    return;

  block_t* b = payload_to_block((block_t*) ptr);

  size_t sz = block_size(b);
 // block_set_size_and_allocated(b, sz, FREE_FLAG);
  block_set_allocated(b, FREE_FLAG);

  block_t* p = block_prev(b);

  // if previous block is free, merge with current block
  while(!block_allocated(p)) {

    pull_free_block(p);
    block_set_size_and_allocated(p, block_size(p) + sz, FREE_FLAG);

    sz = block_size(p);
    b = p;
    p = block_prev(b);
  }

  p = block_next(b);
  sz = block_size(b);

  // if next block is free, merge with current block
  while(!block_allocated(p)) {

    pull_free_block(p);
    block_set_size_and_allocated(b, block_size(p) + sz, FREE_FLAG);

    sz = block_size(b);
    p = block_next(b);
  }

  // return coalesced block
  insert_free_block(b);
}


/*                                               _ _
 *     _ __ ___  _ __ ___      _ __ ___     __ _| | | ___   ___
 *    | '_ ` _ \| '_ ` _ \    | '__/ _ \   / _` | | |/ _ \ / __|
 *    | | | | | | | | | | |   | | |  __/  | (_| | | | (_) | (__
 *    |_| |_| |_|_| |_| |_|___|_|  \___|___\__,_|_|_|\___/ \___|
 *                       |_____|      |_____|
 *
 * reallocates a memory block to update it with a new given size
 * arguments: ptr: a pointer to the memory block
 *            size: the desired new block size
 * returns: a pointer to the new memory block
 */
void *mm_realloc(void *ptr, size_t size) {

  // ptr is NULL
  if(!ptr)
    return mm_malloc(size);

  // size is 0
  if(!size) {
    mm_free(ptr);
    return NULL;
  }

  // find the block containing ptr & size
  block_t* realloc = payload_to_block((block_t*) ptr);
  size_t realloc_sz = block_size(realloc);

  // if realloc'ing to same size
  if(realloc_sz == (size - TAGS_SIZE))
    return realloc->payload;

  // if you're trying to realloc more space than is in block_t* realloc
  if(size >= (realloc_sz - TAGS_SIZE)) {

    // copy contents into temp buffer
    char buffer[realloc_sz];
    memmove(buffer, realloc, realloc_sz);

    // free previous memory so it can possibly be coalesced into larger free block
    mm_free(realloc->payload);

    /* after you've freed the previous block, malloc more space. If there are
     * any blocks of suitable size in the free list, this should look there first */
    block_t* grown = payload_to_block(mm_malloc(size));

    // move contents into new malloc and set its size / allocated flag
    size_t grown_size = block_size(grown);
    memmove(grown, buffer, realloc_sz);
    block_set_size_and_allocated(grown, grown_size, ALLOCATED_FLAG);

    return grown->payload;
  }

  size_t aligned = align(size);

  // if you're shrinking the block and there's enough space to create a new free block
  if(aligned <= (realloc_sz - TAGS_SIZE - MINBLOCKSIZE)) {

    // set size of the new block
    block_set_size_and_allocated(realloc, aligned + TAGS_SIZE, ALLOCATED_FLAG);

    // find block to be reinserted
    block_t* insert = block_next(realloc);
    block_set_size_and_allocated(insert, realloc_sz - aligned - TAGS_SIZE, ALLOCATED_FLAG);

    // free additional memory; use mm_free() to coalesce if possible
    mm_free(insert->payload);

    return realloc->payload;
  }

  // if realloc to smaller block but there isn't enough space create a new free block
  block_t* shrink = payload_to_block(mm_malloc(size));
  size_t shrink_size = block_size(shrink);

  // copy contents into new block
  memmove(shrink, realloc, shrink_size);
  block_set_size_and_allocated(shrink, shrink_size, ALLOCATED_FLAG);

  // free old block
  mm_free(realloc->payload);

  return shrink->payload;
}

/*
 * checks the state of the heap for internal consistency
 * arguments: none
 * returns: 0, if successful
 *          nonzero, if the heap is not consistent
 */
int mm_check_heap(void) {

  // bottom of the heap
  block_t* bottom = mem_heap_lo();
  block_t* first = block_next(bottom);

  // top of the heap
  char* top = (char*) mem_heap_hi() - WORD_SIZE + 1;

  print_heap_info(first, (block_t*) top);
  return check_free_list() | check_free_heap(first, (block_t*) top);
}

/*
 * print out mm_check_heap() info
 */
void print_heap_info(block_t* first, block_t* top) {

  while(first != (block_t*) top) {
    int a = block_allocated(first);
    char* status = a ? "allocated block": "free block";

    // print out info about the heap
    if(a)
      printf("%s at %p, size %zu\n", status, (void*) first, block_size(first));
    else
      printf("%s at %p, size %zu, next: %p\n", status, (void*) first, block_size(first), (void*) block_next_free(first));

    first = block_next(first);
  }
}

/*
 * check that all blocks in the free-list are marked as free; return 0 if successful,
 * 1 otherwise
 */
int check_free_list() {

  block_t* list = flist_first;

  if(list) {
    do {
      if(block_allocated(list)) {
        printf("allocated block %p, in free list\n", (void*) list);
        return 1;
      }

      list = block_next_free(list);

    } while(list != flist_first);
  }

  return 0;
}

/*
 * check that all blocks that are free in the heap are also in the free list
 * will return 1 if a block marked as free is not also in the free list; 0 otherwise
 */
int check_free_heap(block_t* block, block_t* top) {

  while(block != top) {
   if(!block_allocated(block)) {
      if(find_in_list(block))
        return 1;
    }

    block = block_next(block);
  }

  return 0;
}

/*
 * find a block in the free list; return 0 if found, 1 if not
 */
int find_in_list(block_t* block) {
  block_t* initial = flist_first;

  do {
    if(block == initial)
      return 0;

    initial = block_next_free(initial);
  } while(initial != flist_first);

  return 1;
}
