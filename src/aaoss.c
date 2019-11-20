#include <stdio.h>
#include <string.h>

#include "aaoss.h"
#include "disk.h"
#include "utilities.h"

bool expect_numargs(struct command *cmd, const size_t expected) {
  cmd->checked = true;
  size_t numargs = cmd->numitems - 1;
  if (numargs != expected) {
    fprintf(stderr, "Expected %lu arguments to %s command, but got %lu.\n",
            expected, cmd->items[0], numargs);
    return false;
  }

  return true;
}

/* execute a command
 */
void execute(struct process **pcb, struct command *to_run) {
  char *cmd = (to_run->items)[0];
  if (!strcmp(cmd, "A") && expect_numargs(to_run, 2)) {
    long int priority;
    long int size;
    if ((priority = parseInt(to_run->items[1])) != -1 &&
        (size = parseInt((to_run->items)[2])) != -1) {
      process_create(pcb, priority, size);
    }
  } else if (!strcmp(cmd, "fork") && expect_numargs(to_run, 0)) {
    process_fork();
  } else if (!strcmp(cmd, "exit") && expect_numargs(to_run, 0)) {
    process_exit();
  } else if (!strcmp(cmd, "wait") && expect_numargs(to_run, 0)) {
    process_wait();
  } else if (!strcmp(cmd, "d") && expect_numargs(to_run, 2)) {
    long int disk;
    if ((disk = parseInt((to_run->items)[1])) != -1) {
      disk_request(disk, (to_run->items)[2]);
    }
  } else if (!strcmp(cmd, "D") && expect_numargs(to_run, 1)) {
    long int disk;
    if ((disk = parseInt((to_run->items)[1])) != -1) {
      disk_done(disk);
    }
  } else if (!strcmp(cmd, "S") && expect_numargs(to_run, 1)) {
    show((to_run->items)[1]);
  } else if (!to_run->checked) {
    fprintf(stderr, "Command not recognized.\n");
  }
}

int main() {
  char *input;
  struct process *pcb = NULL;

  while ((input = get_input())) {
    struct command to_run = tokenize(input);
    execute(&pcb, &to_run);

    free(to_run.items);
    free(input);
  }

  return 0;
}
