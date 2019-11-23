#include <stdio.h>

#include "aaoss.h"

static void show_die(const char *op) {
  fprintf(stderr, "Unknown argument %s to S command.\n", op);
}

static void show_processes(const struct process *const *pcb) {
  if (*pcb) {
    printf("  %6s %6s %2c\n", "PID", "PR", 'S');
    process_show(*pcb, 'R');

    for (struct process *it = (*pcb)->next; it; it = it->next) {
      process_show(it, 'S');
    }
  }
}

void show(const struct process *const *pcb, const char *op) {
  if (op[1]) { // more than one character argument, abort
    show_die(op);
    return;
  }

  switch (*op) {
  case 'r':
    show_processes(pcb);
    break;
  case 'i':
    break;
  case 'm':
    break;
  default:
    show_die(op);
  }
}
