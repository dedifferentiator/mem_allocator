#include <stdio.h>
#include "unistd.h"
#include <assert.h>


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
Block *heapStart = NULL;

//Current top. Updated on each allocation.
Block *top;

//aligns size by word size (depends on architecture) 32-bit - 4 bytes, 64-bit - 8 bytes
size_t align(size_t size) {
  return (size + sizeof(void *) - 1) &~ (sizeof(void *) -1);
}

//calculates required size for creating new block
size_t alloc_size(size_t size) {
  //size of Block struct - size of pointer included in user data + size of user data
  return sizeof(Block) - sizeof(void *) + size;
}

//allocates memory of size 'size'
Block *request_from_os(size_t size) {
  Block *b = (Block*) sbrk(0);
  
  if (sbrk(alloc_size(size)) == (void *) - 1) {
    return NULL;
  }

  return b;
}

Block *get_header(void *ptr) {
  return (Block *)(ptr - sizeof(Block) + sizeof(void *));
}

//merges two adjacent blocks, addr1 block should be before addr2 block
Block *join_blocks(void *addr1, void *addr2) {
  Block *block1 = get_header(addr1);
  Block *block2 = get_header(addr2);
  block1->size = block1->size + block2->size + sizeof(Block) - sizeof(void *);

  if (block2->next != NULL) {
    Block *block3 = get_header(block2->next);
    block3->prev = block1;
  }

  block1->next = block2->next;
  return block1;
}

//allocates block of memory of at least 'size' bytes
void *mem_alloc(size_t size) {
  size = align(size);
  Block *block = request_from_os(size);
  if (block == NULL) {
    return NULL;
  }
  block->size = size;
  block->used = true;
  block->prev = top;
  block->data = (void *)(((void *) block) + sizeof(Block) - sizeof(void *));

  if (heapStart==NULL) {
    heapStart = block;
  }

  if (top != NULL) {
    top->next = block;
  }

  //updating the most recent block
  top = block; 

  return block->data;
}

//reallocates new block of memory with new size or in case of failure keeps given block unchanged
void *mem_realloc(void *addr, size_t size) {}

//frees allocated memory
void *mem_free(void *addr){
  Block *block = get_header(addr);
  block->used=false;
} 

int main() {
  top = heapStart;

  printf("align: %d\n", align(5));

  void *b1 = mem_alloc(5);
  Block *b1h = get_header(b1);
  assert(b1h->size == sizeof(void *));

  printf("new block ptr: %p size: %d used: %d user data %p\n", b1h, b1h->size, b1h->used, b1h->data);

  void *b2 = mem_alloc(10);
  Block *b2h = get_header(b2);
  assert(b2h->size == sizeof(void *)*2);
  printf("new block ptr: %p size: %d used: %d user data %p\n", b2h, b2h->size, b2h->used, b2h->data);


  void *b3 = mem_alloc(10);
  Block *b3h = get_header(b3);
  assert(b3h->size == sizeof(void *)*2);
  printf("new block ptr: %p size: %d used: %d user data %p\n", b3h, b3h->size, b3h->used, b3h->data);

  mem_free(b2);
  assert(b2h->used==false);
  printf("freed block 2 ptr: %p size: %d used: %d user data %p\n", b2h, b2h->size, b2h->used, b2h->data);

  mem_free(b3);
  assert(b3h->used==false);
  printf("freed block 3 ptr: %p size: %d used: %d user data %p\n", b3h, b3h->size, b3h->used, b3h->data);

  b2h = join_blocks(b2h, b3h);

  printf("joined block 2 ptr: %p size: %d used: %d user data %p\n", b2h, b2h->size, b2h->used, b2h->data);

 
  return 0;
}

