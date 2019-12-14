#ifndef AAOSS_H
#define AAOSS_H

#include <stdbool.h>
#include <stdlib.h>

#include "disk.h"
#include "process.h"

struct command {
  char **items;
  size_t numitems;
  bool checked;
};

char *get_input(const char *prompt);
struct command tokenize(char *string);

bool expect_numargs(struct command *, const size_t);
void execute(struct process *, struct memslab *, struct disks *,
             struct command *);

void show(const struct process *, const struct memslab *, const struct disks *,
          const char *);

const static char SPACE = ' ';

#endif // AAOSS_H
