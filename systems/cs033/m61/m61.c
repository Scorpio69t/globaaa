#define M61_DISABLE 1
#define ALIGNMENT 16
#define INITIAL 0
#define FILE_SIZE 10
#define META 5 * sizeof(size_t)
#define CANARY sizeof(size_t)
#define HH_SIZE 1024

#include "m61.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <stdbool.h>

// global stat struct
struct m61_statistics cstats;

// number of elements in arena
int arena_size = INITIAL;
int arena_capacity = INITIAL;
int arena_actual= INITIAL;

// struct for chaining together free positions
typedef struct free_node {
  int i;
  struct free_node* next;
} free_node;

// struct to keep track of hhs
typedef struct hh_node {
  size_t size;
  char* file;
  int line;
} hh_node;

// create an arena of mallocs
void** arena;

// array of hh info
hh_node* profile[HH_SIZE];

// for linked list
free_node* free_pointer;

// hash into profile; add up ascii values of file name + (HH_SIZE - line #)
int hash(char* file, int line) {
  int counter = 0;
  for(int i = 0; file[i] != '\0'; i++) {
    counter += file[i];
  }

  counter += (HH_SIZE - line);
  return counter % HH_SIZE;
}

void hh_insert(char* file, int line, size_t sz) {
  // get random number; seeded in hhtest.c
  double prob = drand48();
  prob *= 100;

  // insert into profile with a 10% changce
  if(prob < 10) {
    int hh_index = hash((char*) file, line);

    // if not null, update size
    if(profile[hh_index])
      profile[hh_index]->size += sz;
    else {
      // malloc new node
      hh_node* n = malloc(sizeof(hh_node));

      // set initial values
      n->size = sz;
      n->file = (char*) file;
      n->line = line;

      // insert into profile
      profile[hh_index] = n;
    }
  }
}

// find if ptr is a member of arena; O(1)
bool member(void* ptr) {
  // find ptr's index
  size_t* index = ptr;
  int place = *index;

  if(!arena)
    return false;

  // make sure index is valid
  if(place < 0 || place > arena_size)
    return false;

  // make sure ptr matches what's in the arena
  if(arena[place] != ptr)
    return false;

  return true;
}

// insert a pointer into the arena; O(1)
bool arena_insert(void* ptr) {
  if(free_pointer) {
    int place = free_pointer -> i;
    free_node* prev = free_pointer;
    free_pointer = free_pointer -> next;
    free(prev);

    // update ptr's index
    size_t* index = ptr;
    *index = place;

    arena[place] = ptr;
    arena_actual++;
    return true;
  }
  else if (arena_size != arena_capacity) {
    arena[arena_size++] = ptr;
    arena_actual++;
    return true;
  }
  else
    return false;
}

// remove a pointer from the arena; O(1)
bool arena_remove(void* ptr) {
  // find index in arena
  int place = *((size_t*) ptr);

  if (member(ptr)) {
    arena[place] = NULL;
    arena_actual--;
  }
  else
    return false;

  // create new node for free list
  free_node* node = malloc(sizeof(free_node));
  if (!node)
    return false;

  node->next = free_pointer;
  free_pointer = node;
  return true;
}

// make sure all entries in arena are zero after realloc(); O(n)
void arena_zero() {
  for(int i = arena_size; i < arena_capacity; i++) {
    arena[i] = 0x00;
  }
}

// for advanced error messages; O(n)
void* find_ptr_arena(void* ptr) {
  void* possible_ptr = NULL;
  int dif = 0;

  for(int i = 0; i < arena_capacity; i++) {
    // find pointer in arena
    if(arena[i]) {
      // if possible_ptr isn't initialized, set it equal to first ptr
      if(!possible_ptr) {
	possible_ptr = arena[i] + META;
	// update dif but only if ptr is > possible ptr
	if(possible_ptr - ptr > 0)
	  dif = possible_ptr - ptr + META;
	continue;
      }
      else {
	if(ptr - arena[i] > 0 && ptr - arena[i] < dif) {
	  possible_ptr = arena[i] + META;
	  dif = ptr - arena[i] + META;
        }
      }
    }
  }
  return possible_ptr;
}

// free arena; O(1)
void free_arena() {
  if(arena) {
    free(arena);
     arena = NULL;
    return;
  }
  else
    return;
}

// free list of free_nodes; O(n)
void free_list() {
  if(!free_pointer)
    return;
  else {
    free_node* ptr = free_pointer;
    free_pointer = free_pointer -> next;
    free(ptr);
    free_list();
  }
}

// debugging function; O(n)
void arena_print() {
  for(int i = 0; i < arena_capacity; i++)
    printf("arena[%i]: %p\n", i, arena[i]);
}

/// m61_malloc(sz, file, line)
///    Return a pointer to `sz` bytes of newly-allocated dynamic memory.
///    The memory is not initialized. If `sz == 0`, then m61_malloc may
///    either return NULL or a unique, newly-allocated pointer value.
///    The allocation request was at location `file`:`line`.

void* m61_malloc(size_t sz, const char* file, int line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings

    // check for overflow
    if(META + sz < sz) {
      cstats.nfail += 1;
      cstats.fail_size += sz;
      return NULL;
    }

    // allocate a pointer with extra space to hold the size
    void* ptr = base_malloc(META + sz + CANARY);

    // if allocation fails
    if(!ptr) {
      cstats.nfail += 1;
      cstats.fail_size += sz;
      return NULL;
    }

    // update ptr's index
    size_t* index = ptr;
    *index = arena_size;

    // store size of pointer
    size_t* size_region = ptr + sizeof(size_t);
    *size_region = sz;

    // copy file name into metadata
    char* file_region = ptr + (2 * sizeof(size_t));
    memcpy(file_region, file, FILE_SIZE);

    // copy line number into metadata
    int* line_region = ptr + (4 * sizeof(size_t));
    *line_region = line;

    // place canary value onto the end of malloc'd memory
    char* canary = ptr + META + sz;
    char name[] = {"daven"};
    memcpy(canary, name, CANARY);

    // upon successful allocation
    cstats.nactive += 1;
    cstats.active_size += sz;
    cstats.ntotal += 1;
    cstats.total_size += sz;

    // if min isn't set, set it (heap grows up)
    if(!cstats.heap_min)
      cstats.heap_min = (void*) ptr;
    else{
      if((char*) ptr < cstats.heap_min)
        cstats.heap_min = ptr;
    }

    // insert data into profile
    hh_insert((char*) file, line, sz);

    /*** ARENA START ***/

    if(!arena_capacity) {
      arena_capacity++;
      arena = realloc(arena, sizeof(void*));
      arena_zero();
    }

    if(arena_size == arena_capacity) {
      arena_capacity *= 2;
      arena = realloc(arena, arena_capacity * sizeof(void*));
      arena_zero();
    }

    if(!arena_insert(ptr)) {
      printf("Could not place pointer %p in arena\n", ptr);
      abort();
    }

    /*** ARENA STOP ***/

    // change max to point to top of heap; align correctly
    size_t total_growth = META + sz + CANARY;
    char* heap_max = ptr + total_growth + ((ALIGNMENT - (total_growth % ALIGNMENT)) % ALIGNMENT);
    if(heap_max > cstats.heap_max)
      cstats.heap_max = heap_max;

    // return pointer to payload
    return (ptr + META);
}


/// m61_free(ptr, file, line)
///    Free the memory space pointed to by `ptr`, which must have been
///    returned by a previous call to m61_malloc and friends. If
///    `ptr == NULL`, does nothing. The free was called at location
///    `file`:`line`.

void m61_free(void *ptr, const char *file, int line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings

    // prevent free of NULL pointer
    if(!ptr)
      return;

    // prevent free of pointer not allocated
    if(!cstats.heap_min && !cstats.heap_max && ptr) {
      printf("MEMORY BUG: %s:%i: invalid free of pointer %p, not in heap\n", file, line, ptr);
      abort();
    }

    if(!arena_remove(ptr - META)) {
      printf("MEMORY BUG: %s:%i: invalid free of pointer %p, not allocated\n", file, line, ptr);


      void* pos_ptr = find_ptr_arena(ptr);
      if(pos_ptr) {
	size_t* size_region = pos_ptr - (4 * sizeof(size_t));
	int* line_region = pos_ptr - sizeof(size_t);
	char* file_region = pos_ptr - (3 * sizeof(size_t));
	int pos = ptr - pos_ptr;
        printf("  %s:%i: %p is %i bytes inside a %zu byte region allocated here\n", file_region, *line_region, ptr, pos, *size_region);
      }

      abort();
    }

    // prevent free of pointer not in heap
    if((char*) ptr < cstats.heap_min ||  (char*) ptr > cstats.heap_max) {
      printf("MEMORY BUG: %s:%i: invalid free of pointer %p, not in heap\n", file, line, ptr);
      abort();
    }

    size_t sz = *((size_t*) (ptr - (4 * sizeof(size_t))));

    // check canary value to prevent write past boundary
    if(strcmp((char*) ptr + sz, "daven")) {
      printf("MEMORY BUG: %s:%i: detected wild write during free of pointer %p\n", file, line, ptr);
      abort();
    }

    // update stats
    cstats.nactive -= 1;
    cstats.active_size -= sz;

    if(!arena_actual) {
      free_list();
      free_arena();
      arena_size = INITIAL;
      arena_actual = INITIAL;
      arena_capacity = INITIAL;
    }

    base_free(ptr - META);
}


/// m61_realloc(ptr, sz, file, line)
///    Reallocate the dynamic memory pointed to by `ptr` to hold at least
///    `sz` bytes, returning a pointer to the new block. If `ptr` is NULL,
///    behaves like `m61_malloc(sz, file, line)`. If `sz` is 0, behaves
///    like `m61_free(ptr, file, line)`. The allocation request was at
///    location `file`:`line`.

void* m61_realloc(void* ptr, size_t sz, const char* file, int line) {
    void* new_ptr = NULL;

    // if ptr is NULL
    if(!ptr) {
        new_ptr = m61_malloc(sz, file, line);
        return new_ptr;
    }
    else if(ptr && !sz) {
        m61_free(ptr, file, line);
        return new_ptr;
    }
    else if(!member(ptr - META)) {
        m61_free(ptr, file, line);
        return new_ptr;
    }
    else {
        new_ptr = m61_malloc(sz, file, line);

	// if sz new_ptr < sz ptr, only copy new_ptr's sz over
	if(sz < *((size_t*) (ptr - (4 * sizeof(size_t)))))
	  memcpy(new_ptr, ptr, sz);
	else
	  memcpy(new_ptr, ptr, *((size_t*) (ptr - (4 * sizeof(size_t)))));

        m61_free(ptr, file, line);
        return new_ptr;
    }
}


/// m61_calloc(nmemb, sz, file, line)
///    Return a pointer to newly-allocated dynamic memory big enough to
///    hold an array of `nmemb` elements of `sz` bytes each. The memory
///    is initialized to zero. If `sz == 0`, then m61_malloc may
///    either return NULL or a unique, newly-allocated pointer value.
///    The allocation request was at location `file`:`line`.

void* m61_calloc(size_t nmemb, size_t sz, const char* file, int line) {
    // check for overflow
    if((sz * nmemb) < sz || (sz * nmemb) < nmemb) {
      cstats.nfail += 1;
      return NULL;
    }

    void* ptr = m61_malloc(nmemb * sz, file, line);
    if (ptr)
      memset(ptr, 0, nmemb * sz);
    return ptr;
}


/// m61_getstatistics(stats)
///    Store the current memory statistics in `*stats`.

void m61_getstatistics(struct m61_statistics* stats) {
    // Stub: set all statistics to enormous numbers
    memset(stats, 255, sizeof(struct m61_statistics));

    stats->nactive = cstats.nactive;
    stats->active_size = cstats.active_size;
    stats->ntotal = cstats.ntotal;
    stats->total_size = cstats.total_size;
    stats->nfail = cstats.nfail;
    stats->fail_size = cstats.fail_size;
    stats->heap_min = cstats.heap_min;
    stats->heap_max = cstats.heap_max;
}


/// m61_printstatistics()
///    Print the current memory statistics.

void m61_printstatistics(void) {
    struct m61_statistics stats;
    m61_getstatistics(&stats);

    printf("malloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("malloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}


/// m61_printleakreport()
///    Print a report of all currently-active allocated blocks of dynamic
///    memory.

void m61_printleakreport(void) {
  for(int i = 0; i < arena_capacity; i++) {
    void* ptr = arena[i];
    if(ptr) {
      // find size of memory block
      size_t* size_region = ptr + sizeof(size_t);
      int sz = *size_region;

      // find file
      char* file_region = ptr + (2 * sizeof(size_t));

      // find line
      int* line_region = ptr + (4 * sizeof(size_t));
      int l = *line_region;

      printf("LEAK CHECK: %s:%i: allocated object %p with size %i\n", file_region, l, (ptr + META), sz);
    }
  }
}

// print out heavy hitter statistics if a particular allocation is responsible for greater
// than 10% of all allocations

void m61_printhh(void) {
  for(int i = 0; i < HH_SIZE; i++) {
    hh_node* tmp;
    if(profile[i]) {
      tmp = profile[i];
      float ratio = ((float) tmp->size) / ((float) cstats.total_size * .1);
      if(ratio > .1)
        printf("HEAVY HITTER:%s:%i: %zu bytes (~%f%%)\n", tmp->file, tmp->line, tmp->size, 100 * ratio);
      free(tmp);
    }
  }
}
