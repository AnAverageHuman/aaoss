#include <stdio.h>
#include <string.h>

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
  memset(&(newproc->family), 0, sizeof newproc->family);

  return newproc;
}

struct process *process_create() {
  struct process *newproc = calloc(1, sizeof *newproc);
  return newproc;
}

void process_destroy(struct process *todestroy) {
  memory_destroy(todestroy->memory);
  free(todestroy->family.kids);
  free(todestroy->family.dead);
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
    if (newproc) {
      process_insert(processes, newproc);
      newproc->family.parent = parent;
      newproc->family.childidx = parent->family.numkids;

      struct family *f = &(parent->family);
      f->kids = realloc(f->kids, sizeof *(f->kids) * ++(f->numkids));
      f->kids[f->numkids - 1] = newproc;
    }
  }
}

/* process_exit terminates the process, implementing cascading termination
 */
void process_exit(struct process *processes, struct process *process) {
  if (process) {
    memory_destroy(process->memory); // immediately free fake memory
    process->memory = NULL;

    struct family *f = &(process->family);
    while (f->numkids) {
      process_exit(processes, f->kids[f->numkids - 1]); // request exit
    }

    while (f->numdead) {
      process_wait(process);
    }

    process_remove(process);
    struct process *parent = f->parent;
    if (parent) { // remove from kids, add to dead
      struct family *pf = &(parent->family);
      size_t removepos = f->childidx;
      pf->kids[removepos] = pf->kids[--(pf->numkids)];  // no point in shrink
      pf->kids[removepos]->family.childidx = removepos; // fix replacement id
      pf->dead = realloc(pf->dead, sizeof *(pf->dead) * ++(pf->numdead));
      pf->dead[pf->numdead - 1] = process;

      if (pf->waiting && process_wait(parent)) {
        process_insert(processes, parent);
      }

    } else {
      process_destroy(process);
    }
  }
}

/* returns true if wait successful, false if still waiting
 */
bool process_wait(struct process *parent) {
  if (parent) {
    if (parent->family.numdead) {
      process_destroy(parent->family.dead[--parent->family.numdead]);
      parent->family.waiting = false;
      return true;
    }

    parent->family.waiting = true;
    process_remove(parent);
  }

  return false;
}

void process_show(const struct process *proc, const char state) {
  printf("  %6d %6ld %2c\n", proc->pid, proc->priority, state);
}
