#ifndef PROCESS_H
#define PROCESS_H

#include <stdbool.h>
#include <sys/types.h>

#include "memory.h"

struct family {
  struct process *parent;
  size_t childidx; // where we are in the parent children array
  struct process **kids;
  size_t numkids;
  struct process **dead;
  size_t numdead;
  bool waiting;
};

struct process { // it's a linked list
  pid_t pid;
  struct family family;
  long int priority;
  struct process *prev;
  struct process *next;
  struct memslab *memory;
  char *filename;
};

struct process *process_create(void);
struct process *process_new(struct memslab *, const long int, const size_t);
void process_destroy(struct process *);

void process_insert(struct process *, struct process *);
void process_remove(struct process *);

bool process_fork(struct process *processes, struct memslab *);
bool process_exit(struct process *, struct process *);
bool process_wait(struct process *);

void process_show(const struct process *, const char);

#endif // PROCESS_H
