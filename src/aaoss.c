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
void execute(struct process *pcb, struct memslab *memory, struct disks *disks,
             struct command *to_run) {
  char *cmd = (to_run->items)[0];
  if (!strcmp(cmd, "A") && expect_numargs(to_run, 2)) {
    long int priority;
    long int size;
    if ((priority = parseInt(to_run->items[1])) != -1 &&
        (size = parseInt((to_run->items)[2])) != -1) {
      process_insert(pcb, process_new(memory, priority, size));
    }
  } else if (!strcmp(cmd, "fork") && expect_numargs(to_run, 0)) {
    process_fork(pcb, memory);
  } else if (!strcmp(cmd, "exit") && expect_numargs(to_run, 0)) {
    process_exit(pcb, pcb->next);
  } else if (!strcmp(cmd, "wait") && expect_numargs(to_run, 0)) {
    process_wait(pcb->next);
  } else if (!strcmp(cmd, "d") && expect_numargs(to_run, 2)) {
    long int disk;
    if ((disk = parseInt((to_run->items)[1])) != -1) {
      disk_request(pcb, disks, disk, (to_run->items)[2]);
    }
  } else if (!strcmp(cmd, "D") && expect_numargs(to_run, 1)) {
    long int disk;
    if ((disk = parseInt((to_run->items)[1])) != -1) {
      disk_done(pcb, disks, disk);
    }
  } else if (!strcmp(cmd, "S") && expect_numargs(to_run, 1)) {
    show(pcb, memory, disks, (to_run->items)[1]);
  } else if (!to_run->checked) {
    fprintf(stderr, "Command not recognized.\n");
  }
}

int main() {
  char *input;
  struct process *pcb = process_create();
  size_t numdisks = 10;
  struct disks *disks = disks_create(numdisks); // TODO: ask for value
  size_t memsize = 100000;
  struct memslab *memory = memory_create(memsize); // TODO: ask for value

  while ((input = get_input())) {
    struct command to_run = tokenize(input);
    execute(pcb, memory, disks, &to_run);

    free(to_run.items);
    free(input);
  }

  // free disks
  disks_destroy(pcb, disks);

  // free processes
  while (pcb->next) {
    process_exit(pcb, pcb->next);
  }
  process_destroy(pcb);

  // free memory
  memory_destroy(memory->next);
  memory_destroy(memory);

  return 0;
}
