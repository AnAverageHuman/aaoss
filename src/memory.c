#include "memory.h"

struct memslab *memory_create(const size_t maxsize) {
  struct memslab *ret = malloc(sizeof *ret);
  ret->next = malloc(sizeof *(ret->next));
  ret->next->prev = ret; // set up linked list

  ret->base = 0;
  ret->next->base = maxsize;
  ret->prev = ret->next->next = NULL;
  ret->limit = ret->next->limit = 0;

  return ret;
}

void memory_destroy(struct memslab *todestroy) { free(todestroy); }
