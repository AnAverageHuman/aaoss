#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "process.h"

struct disks *disks_create(const size_t numdisks) {
  struct disks *disks = malloc(sizeof *disks);
  disks->numdisks = numdisks;
  disks->queues = calloc(numdisks, sizeof *(disks->queues));
  return disks;
}

void disks_destroy(struct disks *disks) {
  for (size_t i = 0; i < disks->numdisks; i++) {
    while (disks->queues[i]) {
      process_exit(&(disks->queues[i]));
    }
  }

  free(disks->queues);
  free(disks);
}

void disk_request(struct process **pcb, struct disks *disks, long int disk,
                  const char *filename) {
  struct process *proc = process_dequeue(pcb);

  // copy filename as it'll be freed by the main loop
  size_t memsize = sizeof *(proc->filename) * strlen(filename) + 1; // 1 for NUL
  proc->filename = realloc(proc->filename, memsize);
  strcpy(proc->filename, filename);

  struct process *tmp = disks->queues[disk];
  if (tmp) {
    while (tmp->next) {
      tmp = tmp->next;
    }

    tmp->next = proc;
  } else {
    disks->queues[disk] = proc;
  }

  proc->prev = tmp;
}

void disk_done(struct process **pcb, struct disks *disks, long int disk) {
  struct process *proc = process_dequeue(&(disks->queues[disk]));
  if (proc) {
    process_insert(pcb, proc);
  }
}

void disk_show(const struct process *queue, const size_t disknum) {
  if (queue) {
    printf("Disk %lu:\n", disknum);
    for (struct process *tmp = queue; tmp; tmp = tmp->next) {
      printf("  %6d %2c %2s\n", tmp->pid, 'D', tmp->filename);
    }
  }
}
