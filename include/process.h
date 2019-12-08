#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

#include "memory.h"

extern pid_t nextpid;

struct process { // it's a linked list
  pid_t pid;
  pid_t *children;
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

void process_fork(struct process *processes, struct memslab *);
void process_exit(struct process *processes);
void process_wait(void);

void process_show(const struct process *, const char);

#endif // PROCESS_H
