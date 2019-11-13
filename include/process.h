#ifndef PROCESS_H
#define PROCESS_H

void process_create(const long int, const long int);
void process_fork(void);
void process_exit(void);
void process_wait(void);

#endif // PROCESS_H
