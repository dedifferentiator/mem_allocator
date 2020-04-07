#include "stdio.h"
#include "slab.h"

int main() {
  slab *s = init_slab(1024);
  pprint(s, 1);
}
