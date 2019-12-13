#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>

struct memslab { // it's a linked list
  size_t base;
  size_t limit;
  struct memslab *prev;
  struct memslab *next;
  struct process *proc;
};

struct memslab *memory_create(const size_t);
void memory_destroy(struct memslab *);

struct memslab *memory_insert(struct memslab *, const size_t);

void memory_show(const struct memslab *);

#endif // MEMORY_H
