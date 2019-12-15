#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include "aaoss.h"
#include "disk.h"

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

/* prerequisite: string needs to not start with spaces
 */
static size_t parseInt(const char *string, jmp_buf env) {
  size_t ret = 0;
  if (!string || !sscanf(string, "%zu", &ret)) {
    fprintf(stderr, "Expected a positive number, but got %s.\n", string);
    longjmp(env, 1);
  }

  return ret;
}

/* execute a command
 */
void execute(struct process *pcb, struct memslab *memory, struct disks *disks,
             struct command *to_run) {
  jmp_buf initial;
  if (!to_run || !to_run->items || setjmp(initial)) {
    return;
  }

  char *cmd = (to_run->items)[0];
  if (!strcmp(cmd, "A") && expect_numargs(to_run, 2)) {
    size_t priority = parseInt(to_run->items[1], initial);
    size_t size = parseInt((to_run->items)[2], initial);
    process_insert(pcb, process_new(memory, priority, size));
  } else if (!strcmp(cmd, "fork") && expect_numargs(to_run, 0)) {
    process_fork(pcb, memory);
  } else if (!strcmp(cmd, "exit") && expect_numargs(to_run, 0)) {
    process_exit(pcb, pcb->next);
  } else if (!strcmp(cmd, "wait") && expect_numargs(to_run, 0)) {
    process_wait(pcb->next);
  } else if (!strcmp(cmd, "d") && expect_numargs(to_run, 2)) {
    size_t disk = parseInt((to_run->items)[1], initial);
    disk_request(pcb, disks, disk, (to_run->items)[2]);
  } else if (!strcmp(cmd, "D") && expect_numargs(to_run, 1)) {
    size_t disk = parseInt((to_run->items)[1], initial);
    disk_done(pcb, disks, disk);
  } else if (!strcmp(cmd, "S") && expect_numargs(to_run, 1)) {
    show(pcb, memory, disks, (to_run->items)[1]);
  } else if (!to_run->checked) {
    fprintf(stderr, "Command not recognized.\n");
  }
}

_Noreturn static void cleanup(struct process *pcb, struct memslab *memory,
                              struct disks *disks, int ec) {
  // free disks
  disks_destroy(pcb, disks);

  // free processes
  while (pcb->next) {
    process_exit(pcb, pcb->next);
  }
  process_destroy(pcb);

  // free memory
  if (memory) {
    memory_destroy(memory->next);
    memory_destroy(memory);
  }

  exit(ec);
}

inline static size_t read_uint(struct process *pcb, struct memslab *memory,
                               struct disks *disks) {
  size_t out;
  switch (scanf("%zu", &out)) {
  case 0:
    printf("Expected a number, but got something else\n");
    cleanup(pcb, memory, disks, 1);
  case EOF:
    cleanup(pcb, memory, disks, 0);
  }

  int c;
  while ((c = getchar()) != EOF && c != '\n') {
  } // drop rest of line

  return out;
}

int main() {
  char *input;
  struct process *pcb = process_create();

  struct memslab *memory = NULL;
  struct disks *disks = NULL;

  printf("How much memory: ");
  size_t memsize = read_uint(pcb, memory, disks);
  memory = memory_create(memsize);

  printf("How many hard disks: ");
  size_t numdisks = read_uint(pcb, memory, disks);
  disks = disks_create(numdisks);
  if (!disks) {
    fprintf(stderr, "disks_create() failed\n");
    cleanup(pcb, memory, disks, 1);
  }

  while ((input = get_input(">>"))) {
    struct command to_run = tokenize(input);
    execute(pcb, memory, disks, &to_run);

    free(to_run.items);
    free(input);
  }

  cleanup(pcb, memory, disks, 0);
}
