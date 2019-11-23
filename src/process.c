#include <stdio.h>

#include "aaoss.h"
#include "process.h"

pid_t nextpid = 1;

struct process *process_create(const long int priority, const long int size) {
  struct process *newproc = malloc(sizeof *newproc);
  newproc->pid = nextpid++;
  newproc->children = NULL;
  newproc->priority = priority;

  return newproc;
}

void process_destroy(struct process *todestroy) { free(todestroy); }

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

void process_fork(struct process **processes) {
  struct process *parent = *processes;
  struct process *newproc =
      process_create(parent->priority, 0); // TODO: get size of parent
  process_insert(processes, newproc);
}

void process_exit(struct process **processes) {
  if (*processes) {
    struct process *tmp = *processes;
    *processes = tmp->next;
    process_destroy(tmp);
  }
}

void process_wait() {}

void process_show(const struct process *proc, const char state) {
  printf("  %6d %6ld %2c\n", proc->pid, proc->priority, state);
}
