#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "process.h"

struct disks *disks_create(const size_t numdisks) {
  struct disks *disks = malloc(sizeof *disks);
  disks->numdisks = numdisks;
  disks->queues = malloc(numdisks * sizeof *(disks->queues));
  for (size_t i = 0; i < numdisks; i++) {
    disks->queues[i] = process_create();
  }
  return disks;
}

void disks_destroy(struct process *processes, struct disks *disks) {
  for (size_t i = 0; i < disks->numdisks; i++) {
    while (disks->queues[i]->next) {
      process_exit(processes, disks->queues[i]->next);
    }

    process_destroy(disks->queues[i]);
  }

  free(disks->queues);
  free(disks);
}

void disk_request(struct process *pcb, struct disks *disks, long int disk,
                  const char *filename) {
  struct process *proc = pcb->next;
  if (!proc || disks->numdisks <= disk) {
    return;
  }
  process_remove(proc);

  // copy filename as it'll be freed by the main loop
  size_t memsize = sizeof *(proc->filename) * strlen(filename) + 1; // 1 for NUL
  proc->filename = realloc(proc->filename, memsize);
  strcpy(proc->filename, filename);

  struct process *tmp = disks->queues[disk];
  for (; tmp->next; tmp = tmp->next) {
  }

  tmp->next = proc;
  proc->prev = tmp;
}

void disk_done(struct process *pcb, struct disks *disks, long int disk) {
  if (disks->numdisks <= disk) {
    return;
  }

  struct process *proc = disks->queues[disk]->next;
  if (!proc) {
    return;
  }

  process_remove(proc);
  process_insert(pcb, proc);
}

void disk_show(const struct process *queue, const size_t disknum) {
  if (queue->next) {
    printf("Disk %lu:\n", disknum);
    for (struct process *tmp = queue->next; tmp; tmp = tmp->next) {
      printf("  %6d %2c %2s\n", tmp->pid, 'D', tmp->filename);
    }
  }
}
