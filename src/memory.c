#include <stdio.h>

#include "memory.h"
#include "process.h"

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

void memory_destroy(struct memslab *todestroy) {
  if (todestroy && todestroy->prev) {
    todestroy->prev->next = todestroy->next;
  }

  if (todestroy && todestroy->next) {
    todestroy->next->prev = todestroy->prev;
  }

  free(todestroy);
}

/* insert a new memslab into our linked list using worst fit strategy
 */
struct memslab *memory_insert(struct memslab *memory, const size_t limit) {
  struct memslab *candidate = NULL;
  size_t maxhole = 0;

  for (struct memslab *tmp = memory; tmp->next; tmp = tmp->next) {
    size_t newhole = tmp->next->base - tmp->base - tmp->limit;

    if (newhole > limit && newhole > maxhole) {
      candidate = tmp;
      maxhole = newhole;
    }
  }

  if (candidate) {
    struct memslab *newmem = malloc(sizeof *newmem);
    newmem->next = candidate->next;
    candidate->next->prev = newmem;
    newmem->prev = candidate;
    candidate->next = newmem;
    newmem->base = candidate->base + candidate->limit; // left align
    newmem->limit = limit;
    return newmem;
  }

  return NULL;
}

void memory_show(const struct memslab *ms) {
  size_t memend = ms->base + (ms->limit ? ms->limit - 1 : 0);
  printf("  %11lu %11lu %6d\n", ms->base, memend, ms->proc->pid);
}
