#include <stdlib.h>
#include <string.h>

#include "aaoss.h"

int main() {
  char *input;
  while ((input = get_input())) {
    struct command to_run = tokenize(input);

    free(to_run.items);
    free(input);
  }
  return 0;
}
