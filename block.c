#include "block.h"
#include "unistd.h"
#include "stdio.h"

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

//returns header of mem.block by pointer on user data
Block *get_header(void *ptr) {
  return (Block *)(ptr - sizeof(Block) + sizeof(void *));
}

//return size of user data in block
size_t get_size(Block *block) {
  return block->size;
}

//return status of usage of block
bool get_used(Block *block) {
  return block->used;
}

//return pointer to previous block, if it does not exist returns NULL
Block *get_prev(Block *block) {
  return block->prev;
}

//return pointer to next block, if it does not exist return NULL
Block *get_next(Block *block) {
  return block->next;
}

//return pointer to user data
void *get_data(Block *block) {
  return block->data;
}

//sets size of user data
void *set_size(Block *block, size_t size) {
  block->size = size;
}

//sets status of block usage
void *set_used(Block *block, bool used) {
  block->used = used; 
}

//sets pointer to previous block
void *set_prev(Block *block, Block *prev) {
  block->prev=prev;
}

//sets pointer to next block
void *set_next(Block *block, Block *next) {
  block->next = next;
}

//sets pointer to user data
void *set_data(Block *block, void *data) {
  block->data = data;
}

//pretty prints Block struct
void pprint(Block *b, int b_num) {
  printf("block #%d: %p {\n\tsize: %d,\n\tused: %d, \n\tprev: %p, \n\tnext: %p,\n\tdata: %p\n}\n", b_num, b, get_size(b), get_used(b), get_prev(b), get_next(b), get_data(b));
}
