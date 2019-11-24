#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

extern pid_t nextpid;

struct process { // it's a linked list
  pid_t pid;
  pid_t *children;
  long int priority;
  struct process *prev;
  struct process *next;
  char *filename;
};

struct process *process_create(const long int, const long int);
void process_destroy(struct process *);

void process_insert(struct process **processes, struct process *);

void process_fork(struct process **processes);
void process_exit(struct process **processes);
void process_wait(void);

void process_show(const struct process *, const char);

#endif // PROCESS_H
