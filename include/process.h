#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

extern pid_t nextpid;

struct process { // it's a linked list
  pid_t pid;
  pid_t *children;
  long int priority;
  struct process *next;
};

void process_create(struct process **processes, const long int, const long int);
void process_fork(void);
void process_exit(void);
void process_wait(void);

#endif // PROCESS_H
