#include "process.h"
#include "aaoss.h"

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

void process_fork() {}

void process_exit() {}

void process_wait() {}
