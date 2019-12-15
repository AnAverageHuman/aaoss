#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disk.h"
#include "process.h"

struct disks *disks_create(const size_t numdisks) {
  if (numdisks > 100000) { // sanity check
    return NULL;
  }

  struct disks *disks = malloc(sizeof *disks);
  disks->numdisks = numdisks;
  disks->queues = malloc(numdisks * sizeof *(disks->queues));
  for (size_t i = 0; i < numdisks; i++) {
    disks->queues[i] = process_create();
  }
  return disks;
}

void disks_destroy(struct process *processes, struct disks *disks) {
  if (!processes || !disks) {
    return;
  }

  for (size_t i = 0; i < disks->numdisks; i++) {
    while (disks->queues[i]->next) {
      process_exit(processes, disks->queues[i]->next);
    }

    process_destroy(disks->queues[i]);
  }

  free(disks->queues);
  free(disks);
}

bool disk_request(struct process *pcb, struct disks *disks, const size_t disk,
                  const char *filename) {
  struct process *proc = pcb->next;
  if (!proc || disks->numdisks <= disk) {
    return false;
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
  return true;
}

bool disk_done(struct process *pcb, struct disks *disks, size_t disk) {
  if (disks->numdisks <= disk) {
    return false;
  }

  struct process *proc = disks->queues[disk]->next;
  if (!proc) {
    return false;
  }

  process_remove(proc);
  process_insert(pcb, proc);
  return true;
}

void disk_show(const struct process *queue, const size_t disknum) {
  if (queue->next) {
    printf("Disk %lu:\n", disknum);
    for (struct process *tmp = queue->next; tmp; tmp = tmp->next) {
      printf("  %6d %2c %2s\n", tmp->pid, 'D', tmp->filename);
    }
  }
}
