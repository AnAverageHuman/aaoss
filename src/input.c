#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aaoss.h"

/* print a simple prompt to indicate that we are ready to receive input
 */
void print_prompt() { printf(">> "); }

/* prompts for and returns a line of input from stdin, excluding empty lines
 */
char *get_input() {
  char *input = NULL;
  while (!input) {
    print_prompt();

    if (scanf("%m[^\n]", &input) == EOF) {
      free(input);
      return NULL;
    }

    assert(!scanf("%*c")); // discard newline
  }

  return input;
}

/* split a string on spaces into a command structure
 */
struct command tokenize(char *string) {
  struct command ret = {NULL, 0, 0};
  char *tmpbuffer = NULL;
  for (;; string = NULL) {
    if ((tmpbuffer = strtok(string, &SPACE)) == NULL) {
      return ret;
    }

    ret.items = realloc(ret.items, sizeof(char *) * ++ret.numitems);
    ret.items[ret.numitems - 1] = tmpbuffer;
  }
}
