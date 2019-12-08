#include <stdio.h>

#include "aaoss.h"
#include "process.h"

pid_t nextpid = 1;

struct process *process_new(struct memslab *memory, const long int priority,
                            const size_t size) {
  struct memslab *newmem = memory_insert(memory, size);
  if (!newmem) { // failed to allocate fake memory
    return NULL;
  }

  struct process *newproc = calloc(1, sizeof *newproc);
  newproc->pid = nextpid++;
  newproc->priority = priority;
  newproc->memory = newmem;

  return newproc;
}

struct process *process_create() {
  struct process *newproc = calloc(1, sizeof *newproc);
  return newproc;
}

void process_destroy(struct process *todestroy) {
  memory_destroy(todestroy->memory);
  free(todestroy->filename);
  free(todestroy);
}

void process_insert(struct process *processes, struct process *newproc) {
  if (!newproc) {
    return;
  }

  struct process *tmp = processes;
  // using <= : we don't want to preempt processes with same priority
  while (tmp->next && newproc->priority <= tmp->next->priority) {
    tmp = tmp->next;
  }

  if (tmp->next) {
    tmp->next->prev = newproc;
  }

  newproc->next = tmp->next;
  newproc->prev = tmp;
  tmp->next = newproc;
}

void process_remove(struct process *process) {
  if (process && process->prev) {
    process->prev->next = process->next;
  }

  if (process && process->next) {
    process->next->prev = process->prev;
  }

  process->next = process->prev = NULL;
}

void process_fork(struct process *processes, struct memslab *memory) {
  struct process *parent = processes->next;
  if (parent) {
    struct process *newproc =
        process_new(memory, parent->priority, parent->memory->limit);
    process_insert(processes, newproc);
  }
}

void process_exit(struct process *processes) {
  struct process *todestroy = processes->next;
  if (todestroy) {
    process_remove(todestroy);
    process_destroy(todestroy);
  }
}

void process_wait() {}

void process_show(const struct process *proc, const char state) {
  printf("  %6d %6ld %2c\n", proc->pid, proc->priority, state);
}
