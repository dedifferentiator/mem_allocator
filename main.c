#include "stdio.h"
#include "unistd.h"
#include "assert.h"
#include "block.h"

//merges two adjacent blocks, *block1 block should be before the block at *block2
Block *join_blocks(Block *block1, Block *block2) {
  block1->size = get_size(block1) + get_size(block2) + sizeof(Block) - sizeof(void *);

  if (get_next(block2) != NULL) {
    Block *block3 = get_header(get_next(block2));
    block3->prev = block1;
  }

  block1->next = get_next(block2);
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

