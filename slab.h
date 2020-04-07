#include "unistd.h"

#define true 1;
#define false 0;
typedef int bool;

enum slab_t {
  BASIC,
  START,
  CONTI,
  END
};

typedef struct slab {
  enum slab_t type;
  size_t b_size;
  struct slab *prev;
  struct slab *next;
  void *next_free;
} slab;

static size_t slab_size = 4096;
static slab *heap_start = NULL;
static slab *top = NULL;
static bool DEBUG = true;

// aligns size by size of machine word
size_t align(size_t size);

// calculates required size for creating new block
size_t alloc_size(size_t size);

// allocates memory of size 'size'
slab *request_from_os(size_t size);

slab *init_slab(size_t size);

void *set_type(slab *s, enum slab_t t);

void *set_size(slab *s, size_t size);

void *set_prev(slab *s, slab *prev);

void *set_next(slab *s, slab *next);

void *set_next_free(slab *s, slab *next_free);

enum slab_t get_type(slab *s);

size_t get_size(slab *s);

slab *get_prev(slab *s);

slab *get_next(slab *s);

void *get_next_free(slab *s);

// sets pointer to next free block during slab mark-up
void set_next_free_block(void *p1, void *p2);

// returns pointer to next free block in the slab
void *get_free_block(void **p);

// pretty prints slab and its number in list
void pprint(slab *s, size_t num);

// pretty prints slab layout
void pprint_layout(slab *s);
