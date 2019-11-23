#include <stdio.h>

#include "aaoss.h"
#include "process.h"

pid_t nextpid = 1;

void process_create(struct process **processes, const long int priority,
                    const long int size) {
  struct process *newproc = malloc(sizeof *newproc);
  newproc->pid = nextpid++;
  newproc->children = NULL;
  newproc->priority = priority;

  // using <= : we don't want to preempt processes with same priority
  if (*processes && newproc->priority <= (*processes)->priority) {
    struct process *tmp = *processes;
    while (tmp->next && newproc->priority <= tmp->next->priority) {
      tmp = tmp->next;
    }

    newproc->next = tmp->next;
    tmp->next = newproc;
  } else { // no running processes, or new one has largest priority
    newproc->next = *processes;
    *processes = newproc;
  }
}

void process_fork(struct process **processes) {
  struct process *parent = *processes;
  process_create(processes, parent->priority, 0); // TODO: get size of parent
}

void process_exit(struct process **processes) {
  if (*processes) {
    struct process *tmp = *processes;
    *processes = tmp->next;
    free(tmp);
  }
}

void process_wait() {}

void process_show(const struct process *proc) {
  printf("  %6d %6ld \n", proc->pid, proc->priority);
}
