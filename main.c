#include <stdio.h>
#include "unistd.h"

typedef int bool;
#define true 1
#define false 0

typedef struct Block {
  // -------------------------------------
  // 1. Object header

  //Block size.
  size_t size;
  
  // Whether this block is currently used.
  bool used;

  //Next block in the list.
  struct Block *next;
  // -------------------------------------
  // 2. User data
 
  //Payload pointer.
  void* data;

} Block;

//Heap start. Initialized on first allocation.
Block *heapStart = NULL;

//Current top. Updated on each allocation.
Block *top;

//aligns size by word size (depends on architecture) 32-bit - 4 bytes, 64-bit - 8 bytes
size_t align(size_t size) {
  return (size + sizeof(void *) - 1) &~ (sizeof(void *) -1);
}

size_t alloc_size(size_t size) {
  //size of Block struct - size of pointer included in user data + size of user data
  return sizeof(Block) - sizeof(void *) + size;
}

Block *request_from_os(size_t size) {
  Block *b = (Block*) sbrk(0);
  
  if (sbrk(alloc_size(size)) == (void *)-1) {
    return NULL;
  }

  return b;
}

//allocates block of memory of at least 'size' bytes
void *mem_alloc(size_t size) {
  size = align(size);
  Block *block = request_from_os(size);

  block->size = size;
  block->used=true;
  if (heapStart==NULL) {
    heapStart = block;
  }

  if (top != NULL) {
    top->next = block;
  }

  top = block; 

  return block->data;
}

//reallocates new block of memory with new size or in case of failure keeps given block unchanged
void *mem_realloc(void *addr, size_t size) {}

//frees allocated memory
void *mem_free(void *addr){} 

int main() {
  top = heapStart;
  printf("align: %d\n", align(5));
  return 0;
}

