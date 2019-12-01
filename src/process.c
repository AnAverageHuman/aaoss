#include <stdio.h>

#include "aaoss.h"
#include "process.h"

pid_t nextpid = 1;

struct process *process_create(struct memslab *memory, const long int priority,
                               const size_t size) {
  struct process *newproc = malloc(sizeof *newproc);
  newproc->pid = nextpid++;
  newproc->children = NULL;
  newproc->priority = priority;
  newproc->filename = NULL;
  newproc->memory = memory_insert(memory, size);

  return newproc;
}

void process_destroy(struct process *todestroy) {
  memory_destroy(todestroy->memory);
  free(todestroy->filename);
  free(todestroy);
}

void process_insert(struct process **processes, struct process *newproc) {
  // using <= : we don't want to preempt processes with same priority
  if (*processes && newproc->priority <= (*processes)->priority) {
    struct process *tmp = *processes;
    while (tmp->next && newproc->priority <= tmp->next->priority) {
      tmp = tmp->next;
    }

    if (tmp->next) {
      tmp->next->prev = newproc;
    }

    newproc->next = tmp->next;
    newproc->prev = tmp;
    tmp->next = newproc;
  } else { // no running processes, or new one has largest priority
    if (*processes) {
      (*processes)->prev = newproc;
    }

    newproc->next = *processes;
    *processes = newproc;
  }
}

struct process *process_dequeue(struct process **processes) {
  struct process *tmp = *processes;
  if (tmp) {
    *processes = tmp->next;
    tmp->next = tmp->prev = NULL;
  }

  return tmp;
}

void process_fork(struct process **processes, struct memslab *memory) {
  if (*processes) {
    struct process *parent = *processes;
    struct process *newproc =
        process_create(memory, parent->priority, parent->memory->limit);
    process_insert(processes, newproc);
  }
}

void process_exit(struct process **processes) {
  process_destroy(process_dequeue(processes));
}

void process_wait() {}

void process_show(const struct process *proc, const char state) {
  printf("  %6d %6ld %2c\n", proc->pid, proc->priority, state);
}
