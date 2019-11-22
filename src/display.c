#include <stdio.h>

#include "aaoss.h"

static void show_die(const char *op) {
  fprintf(stderr, "Unknown argument %s to S command.\n", op);
}

void show(const char *op) {
  if (op[1]) { // more than one character argument, abort
    show_die(op);
    return;
  }

  switch (*op) {
  case 'r':
    break;
  case 'i':
    break;
  case 'm':
    break;
  default:
    show_die(op);
  }
}
