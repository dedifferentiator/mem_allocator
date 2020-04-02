#include "stdio.h"
#include "unistd.h"
#include "assert.h"
#include "block.h"

//merges two adjacent blocks, *block1 block should be before the block at *block2
Block *join_blocks(Block *block1, Block *block2) {
  set_size(block1, get_size(block1) + get_size(block2) + sizeof(Block) - sizeof(void *));

  if (get_next(block2) != NULL) {
    Block *block3 = get_header(get_next(block2));
    set_prev(block3, block1);
  }
  if (top == block2) {
    top = block1;
  }
  set_next(block1, get_next(block2));
  return block1;
}

void *alloc_block(size_t size) {
  size = align(size);
  Block *block = request_from_os(size);
  if (block == NULL) {
    return NULL;
  }
  set_size(block, size);
  set_used(block, true);
  set_prev(block, top);
  set_data(block, (void *)(((void *) block) + sizeof(Block) - sizeof(void *)));

  if (heapStart == NULL) {
    heapStart = block;
  }

  if (top != NULL) {
    set_next(top, block);
    //if previous block exists set *next field of it to current block
    Block *block_prev = get_header(get_prev(block));
    set_next(block_prev, block);
  }

  //updating the most recent block
  top = block; 

  return get_data(block);
}

//searches for block with free space with at least 'size' bytes
Block *find_block(size_t size) {
  if (heapStart == NULL) {
    return NULL;
  }
  
  Block *cur_block = heapStart;
  printf("got here");
  while (cur_block != NULL) {
    printf("got here 1");

    if (get_size(cur_block) >= size && !get_used(cur_block)) {
      return cur_block;
    }
    cur_block = get_next(cur_block);
    printf("got here 2");
  }
  printf("got here 3");

  return NULL;
}

//spliting block on two block, first one has a size of user data at least 'size_u' if splitting is impossible returns block unchanged
Block *split_block(Block *block, size_t size_u) {
  //printf("split_block has started...\n");
  //printf("Total: %d, Available size: %d\n", get_size(block), ((int)get_size(block) - (int)sizeof(Block) - (int)sizeof(void *)*2));
  //printf("Aligned required size: %d\n", align(size_u));

  if (((int)get_size(block) - (int)sizeof(Block) - (int)sizeof(void *)*2) >= (int)align(size_u) && !get_used(block)) {
    //shifting right to create new
    //printf("resizing...\n");
    Block *r_block = (Block *)((void *)block + sizeof(Block) - sizeof(void *) + align(size_u));

    set_size(r_block, get_size(block) - sizeof(Block));
    set_used(r_block, false);
    set_prev(r_block, block);
    set_next(r_block, get_next(block));
    set_data(r_block, (void *)(((void *) r_block) + sizeof(Block) - sizeof(void *)));

    set_next(block, r_block);
    set_size(block, align(size_u));

    if (top == block) {
      top = r_block;
    }
  }
  return block;
}

//allocates block of memory of at least 'size' bytes
void *mem_alloc(size_t size) {
  printf("got here NUL");

  Block *block = find_block(size);
  
  if (block != NULL) {
    Block *new_block = split_block(block, size);
    set_used(new_block, true);
    return get_data(new_block);
  }
  return alloc_block(size);
}

//reallocates new block of memory with new size or in case of failure keeps given block unchanged
void *mem_realloc(void *addr, size_t size) {}

//frees allocated memory
void *mem_free(void *addr){
  Block *block = get_header(addr);
  block->used=false;

  if (get_prev(block) != NULL && !get_used(get_prev(block))) {
    block = join_blocks(get_prev(block), block);
  }
  if (get_next(block) != NULL && !get_used(get_next(block))) {
    block = join_blocks(block, get_next(block));
  }
}
 
int main() {
  top = heapStart;

  assert(sizeof(void *) == align(5));
  assert(sizeof(void *) * 5 == align(40));

  void *b1u = mem_alloc(5);
  Block *b1 = get_header(b1u);

  assert(get_size(b1) == sizeof(void *));
  pprint(b1, 1);
  
  void *b2u = mem_alloc(10);
  Block *b2 = get_header(b2u);

  assert(get_size(b2) == sizeof(void *)*2);
  pprint(b1, 1);

  pprint(b2, 2);

  void *b3u = mem_alloc(10);
  Block *b3 = get_header(b3u);

  assert(get_size(b3) == sizeof(void *)*2);

  pprint(b1, 1);
  pprint(b2, 2);
  pprint(b3, 3);

  void *b4u = mem_alloc(10);
  Block *b4 = get_header(b4u);

  assert(get_size(b4) == sizeof(void *)*2);

  pprint(b1, 1);
  pprint(b2, 2);
  pprint(b3, 3);
  pprint(b4, 4);


/*  mem_free(b2u);
  assert(get_used(b2) == false);

  mem_free(b3u);
  assert(get_used(b3) == false);

  printf("AFTER FREEING BLOCK 2 AND BLOCK 3\n");
  pprint(b1, 1);
  pprint(b2, 2);
  pprint(b3, 3);

  b2u = mem_alloc(5);
  b2 = get_header(b2u);
  assert(b2->size == sizeof(void *));

  printf("AFTER REQUESTING NEW MEMORY\n");
  pprint(b1, 1);
  pprint(b2, 2);
  pprint(get_next(b2), 3);

  printf("TOP: %p\n", top);
  void *b4u = mem_alloc(24);
  Block *b4 = get_header(b4u);
  assert(b4->size == sizeof(void *) * 3);

  printf("AFTER REQUESTING NEW MEMORY\n");
  pprint(b1, 1);
  pprint(b2, 2);
  pprint(get_next(b2), 3);
  pprint(b4, 4);
*/
  return 0;
}

