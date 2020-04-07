#include "slab.h"
#include "unistd.h"
#include "stdio.h"


size_t align(size_t size) {
  return (size + sizeof(void *) - 1) &~ (sizeof(void *) -1);
}

size_t alloc_size(size_t size) {
  // size of Block struct - size of pointer included in user data + size of user data
  return sizeof(slab) - sizeof(void *) + size;
}

slab *request_from_os(size_t size) {
  slab *b = (slab *) sbrk(0);

  if (sbrk(alloc_size(size)) == (void *) - 1) {
    return NULL;
  }
  return b;
}

slab *init_slab(size_t size) {
  size = align(size);

  if (size > slab_size / 2) {
    // TODO: should create start,continius and end slabs and return pointer to the first one
    return NULL;
  }

  slab *s = request_from_os(slab_size);
  if (!s) {
    return NULL;
  }

  set_type(s, BASIC);
  set_size(s, size);
  set_next(s, NULL);
  set_next_free(s, (void *)((size_t)s + size));
  if (!top) {
    set_prev(s, NULL);
  }
  set_prev(s, top);

  // slab marking up
  void *next_p = NULL;
  for (size_t i=(size_t)s  + slab_size + sizeof(slab) - sizeof(void *) - size; i>=(size_t)s; i-=size) {
    set_next_free_block((void *)i, next_p);
    next_p = (void *) i;
  }
  
  if (DEBUG) {
    pprint_layout(s);
  }
  
  //update slabs top
  top = s;

  return s;
}


void *set_type(slab *s, enum slab_t t) {
  s->type = t;
}

void *set_size(slab *s, size_t size) {
  s->b_size=size;
}

void *set_prev(slab *s, slab *prev) {
  s->prev = prev;
}

void *set_next(slab *s, slab *next) {
  s->next = next;
}

void *set_next_free(slab *s, slab *next_free) {
  s->next_free = next_free;
}

enum slab_t get_type(slab *s) {
  return s->type;
}

size_t get_size(slab *s) {
  return s->b_size;
}

slab *get_prev(slab *s) {
  return s->prev;
}

slab *get_next(slab *s) {
  return s->next;
}

void *get_next_free(slab *s) {
  return s->next_free;
}

void set_next_free_block(void *p1, void *p2) {
  *(size_t *)p1 = (size_t) p2;
}

void *get_next_free_block(void **p) {
  return *p;
}

void pprint(slab *s, size_t num) {
  printf("slab #%d: %p {\n\ttype: %d,\n\tprev: %p, \n\tnext: %p, \n\tnext_free: %p}\n", num, s, get_type(s), get_prev(s), get_next(s), get_next_free(s));
}

void pprint_layout(slab *s) {
  printf("======LAYOUT OF %p ======\n", s);
  
  for (size_t i=(size_t)s  + slab_size + sizeof(slab) - sizeof(void *) - get_size(s); i>=(size_t)s; i-=get_size(s)) {
    printf("%p POINTS TO %p\n", (void *) i, get_next_free_block((void *) i));
  }
  printf("===============================\n");
}
