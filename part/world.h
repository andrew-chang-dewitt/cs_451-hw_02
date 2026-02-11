// fix implicit dprintf declaration
// from: https://stackoverflow.com/a/39671290
#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORLDH

void step(char *world_history, const unsigned long size,
          const unsigned long step_number);

void init_step(char *world_history, const unsigned long size,
               const char *init_world);
void print_world(int fd, char *world_history, const unsigned long size,
                 const unsigned long step_number);

char get_value(char *world_history, const unsigned long size,
               const unsigned long step_number, const unsigned long x,
               const unsigned long y);
void set_value(char *world_history, const unsigned long size,
               const unsigned long step_number, const unsigned long x,
               const unsigned long y, const char value);
char neighbour_state(char *world_history, const unsigned long size,
                     const unsigned long step_number, const unsigned long x,
                     const unsigned long y,
                     const unsigned char neighbour_offset);
unsigned char living_neighbours(char *world_history, const unsigned long size,
                                const unsigned long step_number,
                                const unsigned long x, const unsigned long y);
