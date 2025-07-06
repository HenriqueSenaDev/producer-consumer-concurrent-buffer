#include <pthread.h>

#ifndef GLOBALS_H
#define GLOBALS_H

extern int size;
extern int start;
extern int end;
extern int buff_size;
extern int show_buffer;
extern int *buffer;
extern pthread_mutex_t mutex;
extern void print_buffer();

#endif
