#include <stdlib.h>

#include "aaoss.h"

int main() {
  char *input;
  while ((input = get_input())) {
    free(input);
  }
  return 0;
}
