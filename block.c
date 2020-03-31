#include "block.h"
#include "unistd.h"
#include "stdio.h"

Block *heapStart = NULL;

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

//pretty prints Block struct
void pprint(Block *b) {
  printf("block: %p {\n\tsize: %d,\n\tused: %d,\n\tdata: %p\n}\n", b, b->size, b->used, b->data);
}
