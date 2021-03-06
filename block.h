#include "unistd.h"

typedef int bool;
#define true 1
#define false 0

typedef struct Block {
  // Block size.
  size_t size;

  // Whether this block is currently used
  bool used;

  // Previous block in the list
  struct Block *prev;

  // Next block in the list
  struct Block *next;

  // User data
  void* data;

} Block;

// heap start
static Block *heap_start = NULL;

// current top; updated on each allocation
static Block *top = NULL;

// allocates memory of size 'size'
Block *request_from_os(size_t size);

// returns header of mem.block by pointer on user data
Block *get_header(void *ptr);

// return size of user data in block
size_t get_size(Block *block);

// return status of usage of block
bool get_used(Block *block);

// return pointer to previous block, if it does not exist returns NULL
Block *get_prev(Block *block);

// return pointer to next block, if it does not exist return NULL
Block *get_next(Block *block);

// return pointer to user data
void *get_data(Block *block);

// sets size of user data
void *set_size(Block *block, size_t size);

// sets status of block usage
void *set_used(Block *block, bool used);

// sets pointer to previous block
void *set_prev(Block *block, Block *prev);

// sets pointer to next block
void *set_next(Block *block, Block *next);

// sets pointer to user data
void *set_data(Block *block, void *data);

// pretty prints Block struct
void pprint(Block *b, int b_num);

// aligns size by size of machine word
size_t align(size_t size);

// calculates required size for creating new block
size_t alloc_size(size_t size); 
