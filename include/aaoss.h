#ifndef AAOSS_H
#define AAOSS_H

#include <stdbool.h>
#include <stdlib.h>

struct command {
  char **items;
  size_t numitems;
  bool checked;
};

void print_prompt(void);
char *get_input(void);
struct command tokenize(char *string);

bool expect_numargs(struct command *, const size_t);
void execute(struct command *);

void show(const char *);

const static char SPACE = ' ';

#endif // AAOSS_H
