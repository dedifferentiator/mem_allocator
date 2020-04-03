#include "stdio.h"
#include "unistd.h"
#include "assert.h"
#include "block.h"

// merges two adjacent blocks, *block1 block should be before the block at *block2
Block *join_blocks(Block *block1, Block *block2) {
  set_size(block1, get_size(block1) + get_size(block2) + sizeof(Block) - sizeof(void *));
  set_next(block1, get_next(block2));

  if (get_next(block2)) {
    set_prev(get_next(block2), block1);
  }  

  if (top == block2) {
    top = block1;
  }
  return block1;
}

// allocates new block of memory
void *alloc_block(size_t size) {
  size = align(size);
  Block *block = request_from_os(size);
  if (!block) {
    return NULL;
  }

  set_size(block, size);
  set_used(block, true);
  set_prev(block, NULL); // setting pointers to prev and next block explicitly to ensure mem.consistency
  set_next(block, NULL);
  set_data(block, (void *)(((void *) block) + sizeof(Block) - sizeof(void *)));

  if (top) {
    set_prev(block, top);
    set_next(top, block);
  }

  if (!heap_start) {
    heap_start = block;
  }

  // updating the most recent block
  top = block; 

  return get_data(block);
}

// searches for block with free space with at least 'size' bytes
Block *find_block(size_t size) {
  Block *cur_block = heap_start;

  while (cur_block) {
    if (get_size(cur_block) >= size && !get_used(cur_block)) {
      return cur_block;
    }
    cur_block = get_next(cur_block);
  }

  return NULL;
}

// spliting block on two block, first one has a size of user data at least 'size_u' if splitting is impossible returns block unchanged
Block *split_block(Block *block, size_t size_u) {
  Block *r_block = (Block *)((void *)block + sizeof(Block) - sizeof(void *) + size_u);

  set_size(r_block, get_size(block) - sizeof(Block) + sizeof(void *));
  set_used(r_block, false);
  set_prev(r_block, block);
  set_next(r_block, get_next(block));
  set_data(r_block, (void *)(((void *) r_block) + sizeof(Block) - sizeof(void *)));


  set_next(block, r_block);
  set_size(block, align(size_u));

  if (top == block) {
    top = r_block;
  }
  return block;
}

// copying every byte from addr1 to addr2 up to addr1+size byte
void mem_copy(void *addr1, void *addr2, size_t size) {
  char *obj = (char *) addr1;
  char *obj_n = (char *) addr2 + sizeof(void *);
  for (size_t i = 0; i < size; i++) {
    *(obj_n + i) = *(obj + i);
  }
}

// pretty prints block chain
void *mem_dump() {
  Block *cur_b = heap_start;
  size_t b_num = 1;  // block number in the chain

  while (cur_b) {
    pprint(cur_b, b_num);
    printf("=====================\n");

    cur_b = get_next(cur_b);
    b_num++;
  }
}

// allocates block of memory of at least 'size' bytes
void *mem_alloc(size_t size) {
  size = align(size);

  Block *block = find_block(size);
   
  if (block) {
    if (((int)get_size(block) - (int)size - (int)sizeof(Block) + (int)sizeof(void *)) >= (int)(sizeof (void*))) {
      block = split_block(block, size);
    }
    set_used(block, true);
    return get_data(block);
  }
  return alloc_block(size);
}


// reallocates new block of memory with new size or in case of failure keeps given block unchanged
void *mem_realloc(void *addr, size_t size) {
  // if pointer to data was not provided alloc new block
  if (!addr) {
    return mem_alloc(size);
  }

  Block *block = get_header(addr);
  // if block is not allocated return NULL
  if (!get_used(block)) {
    return NULL;
  }

  void *new_addr = mem_alloc(size);
  if (!new_addr) {
    return NULL;
  }

  Block *new_block = get_header(new_addr);

  mem_copy(addr, new_addr, size);
  // after successful copying marking old block as unused
  set_used(block, false);
  return new_addr;
}

// frees allocated memory
void *mem_free(void *addr){
  Block *block = get_header(addr);
  set_used(block, false);

  if (get_prev(block) && !get_used(get_prev(block))) {
    block = join_blocks(get_prev(block), block);
  }
  if (get_next(block) && !get_used(get_next(block))) {
    block = join_blocks(block, get_next(block));
  }
}
 
int main() {
  printf("-----ALLOCATING 3 NEW BLOCKS-----\n");
  assert(sizeof(void *) == align(5));
  assert(sizeof(void *) * 5 == align(40));

  void *b1u = mem_alloc(5);
  Block *b1 = get_header(b1u);
  assert(get_size(b1) == sizeof(void *));

  void *b2u = mem_alloc(10);
  Block *b2 = get_header(b2u);
  assert(get_size(b2) == sizeof(void *)*2);

  void *b3u = mem_alloc(10);
  Block *b3 = get_header(b3u);
  assert(get_size(b3) == sizeof(void *)*2);

  mem_dump(); 

  mem_free(b2u);
  assert(get_used(b2) == false);
  assert(get_used(b1) == true);

  mem_free(b3u);
  assert(get_used(b2) == false);
  assert(get_used(b1) == true);
  printf("-----AFTER FREEING BLOCK 2 AND BLOCK 3-----\n");

  mem_dump();

  void *b4u = mem_alloc(5);
  Block *b4 = get_header(b4u);
  assert(get_size(b4) == sizeof(void *));
  printf("-----AFTER REQUESTING NEW MEMORY-----\n");

  mem_dump();

  void *b5u = mem_alloc(24);
  Block *b5 = get_header(b5u);
  // size should be 32, because block with size 32 cannot be splitted into 2 blocks with one of them 24
  assert(get_size(b5) == sizeof(void *) * 4);
  printf("-----AFTER REQUESTING NEW MEMORY-----\n");

  mem_dump();

  void *b6u = mem_alloc(24);
  Block *b6 = get_header(b6u);
  assert(get_size(b6) == sizeof(void *) * 3);
  printf("-----AFTER REQUESTING NEW MEMORY-----\n");

  mem_dump();

  void *b7u = mem_realloc(b4u, 25);
  Block *b7 = get_header(b7u);
  assert(b7u);
  assert(get_used(b4) == false);
  assert(get_size(b7) == sizeof(void *) * 4);
  printf("-----AFTER REALLOCATING MEMORY-----\n");

  mem_dump();

  void *b8u = mem_realloc(b1u, 25);
  Block *b8 = get_header(b8u);
  assert(b8u);
  assert(get_used(b1) == false);
  assert(get_size(b8) == sizeof(void *) * 4);
  printf("-----AFTER REALLOCATING MEMORY-----\n");

  mem_dump();

  return 0;
}

