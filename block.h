#include "unistd.h"

typedef int bool;
#define true 1
#define false 0

typedef struct Block {
  // -------------------------------------
  // 1. Object header

  //Block size.
  size_t size;

  // Whether this block is currently used
  bool used;

  //Previous block in the list
  struct Block *prev;

  //Next block in the list
  struct Block *next;
  // -------------------------------------

  // 2. User data
  void* data;

} Block;

//Heap start. Initialized on first allocation.
Block *heapStart;

//Current top. Updated on each allocation.
Block *top;

//allocates memory of size 'size'
Block *request_from_os(size_t size);

//returns header of mem.block by pointer on user data
Block *get_header(void *ptr);

//pretty prints Block struct
void pprint(Block *b);

size_t align(size_t size);

size_t alloc_size(size_t size); 
