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
    //if previous block exists set *next field of it to current block
    Block *block_prev = get_header(get_prev(block));
    block_prev->next = block;
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
  pprint(b1h, 1);
  
  void *b2 = mem_alloc(10);
  Block *b2h = get_header(b2);
  assert(b2h->size == sizeof(void *)*2);
  pprint(b2h, 2);

  void *b3 = mem_alloc(10);
  Block *b3h = get_header(b3);
  assert(b3h->size == sizeof(void *)*2);
  pprint(b3h, 3);

  mem_free(b2);
  assert(b2h->used==false);
  pprint(b2h, 2);

  mem_free(b3);
  assert(b3h->used==false);
  pprint(b3h, 3);

  b2h = join_blocks(b2h, b3h);
  pprint(b2h, 2); 
  return 0;
}

