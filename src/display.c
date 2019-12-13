#include <stdio.h>

#include "aaoss.h"
#include "disk.h"

static void show_die(const char *op) {
  fprintf(stderr, "Unknown argument %s to S command.\n", op);
}

static void show_processes(const struct process *pcb) {
  if (pcb->next) {
    printf("  %6s %6s %2c\n", "PID", "PR", 'S');
    process_show(pcb->next, 'R');

    for (struct process *it = pcb->next->next; it; it = it->next) {
      process_show(it, 'S');
    }
  }
}

static void show_memory(const struct memslab *memory) {
  if (memory) {
    printf("  %11s %11s %6s\n", "MSTART", "MEND", "PID");
    // skip first and last boundary slabs
    for (struct memslab *tmp = memory->next; tmp->next; tmp = tmp->next) {
      memory_show(tmp);
    }
  }
}

static void show_disks(const struct disks *disks) {
  printf("  %6s %2c %2s\n", "PID", 'S', "NAME");
  for (size_t i = 0; i < disks->numdisks; i++) {
    disk_show(disks->queues[i], i);
  }
}

void show(const struct process *pcb, const struct memslab *memory,
          const struct disks *disks, const char *op) {
  if (op[1]) { // more than one character argument, abort
    show_die(op);
    return;
  }

  switch (*op) {
  case 'r':
    show_processes(pcb);
    break;
  case 'i':
    show_disks(disks);
    break;
  case 'm':
    show_memory(memory);
    break;
  default:
    show_die(op);
  }
}
