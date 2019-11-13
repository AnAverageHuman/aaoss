#ifndef AAOSS_H
#define AAOSS_H

struct command {
  char **items;
  size_t numitems;
};

void print_prompt(void);
char *get_input(void);
struct command tokenize(char *string);

const static char SPACE = ' ';

#endif // AAOSS_H
