// fix implicit dprintf declaration
// from: https://stackoverflow.com/a/39671290
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef WORLDH
#include "world.h"
#endif

#ifndef INTERFACEH
#include "interface.h"
#endif

/* * * * * * * * * * * * * * * * * * * * * * * *
 * Globals
 * -------
 *
 * includes comparts map, extension id,
 * & world history state
 *
 * declared as extern to be available to all
 * compartments
 * * * * * * * * * * * * * * * * * * * * * * * */

struct compart comparts[NO_COMPARTS] = {{.name = "main compartment",
                                         .uid = 65534,
                                         .gid = 65534,
                                         .path = "/tmp",
                                         .comms = NULL},
                                        {.name = "step compartment",
                                         .uid = 65534,
                                         .gid = 65534,
                                         .path = "/tmp",
                                         .comms = NULL}};

// init public extension identifer pointers as NULL
// they must be redefined by user of this code
struct extension_id *step_ext = NULL;

/* * * * * * * * * * * * * * * * * * * * * * * *
 * Compartment extension fn
 * * * * * * * * * * * * * * * * * * * * * * * */

// execute a step on the given history of states
// shows pattern we'll see across other ext_* functions:
// 1. declare a file descriptor for logging
// 2. deserialize argument data
// 3. perform computation w/ output of (2)
// 4. serialize value to return
// 5. return output of (4)
//
// data {
//   //     8 bytes   8 bytes  4 bytes       size_history bytes
//   //     ul        ul       size_t        char*
//   buf: { step_num, size   , size_history, world_history }
// }
struct extension_data ext_step(struct extension_data data) {
  struct extension_data result;

  unsigned long step_num = 0;
  unsigned long size = 0;
  size_t offset_history = 0;
  size_t size_history = 0;
  char *world_history = NULL;

  // 1. declare a file descriptor for logging
  int fd = -1;
#ifdef LC_ALLOW_EXCHANGE_FD
  fd = STDOUT_FILENO;
#endif
  dprintf(fd, "[%s:ext_step()]:%d BEGIN\n", compart_name(), getuid());

  // 2. deserialize argument data
#ifndef LC_ALLOW_EXCHANGE_FD
  offset_history = ext_nums_from_arg(data, &step_num, &size, &size_history);
#else
  offset_history = ext_nums_from_arg(data, &step_num, &size, &size_history, fd);
#endif
  dprintf(fd, "[%s:ext_step()]:%d numbers extracted (%ld, %ld, %d, %d)\n",
          compart_name(), getuid(), step_num, size, size_history,
          offset_history);

  world_history = malloc(size_history);

#ifndef LC_ALLOW_EXCHANGE_FD
  ext_history_from_arg(data, size_history, world_history);
#else
  ext_history_from_arg(data, world_history, fd);
#endif
  dprintf(fd, "[%s:ext_step()]:%d world_history extracted: ", compart_name(),
          getuid());
  print_world(fd, world_history, size, step_num);

  // 3. perform computation w/ output of (2)
  step(world_history, size, step_num);

  // 4. serialize value to return
  result = ext_step_to_arg(step_num, size, size_history, world_history);

  // 5. return output of (4)
  return result;

  //   unsigned long step_num = 0;
  //   unsigned long size = 0;
  //   int offset = -1;
  //
  //   // 1. declare a file descriptor for logging
  //   int fd = -1;
  // #ifndef LC_ALLOW_EXCHANGE_FD
  //   fd = STDOUT_FILENO;
  //   // dprintf(fd, "[%s]:%d BEGIN\n", compart_name(), getuid());
  //   dprintf(fd, "[%s:ext_step()]:%d BEGIN\n", compart_name(), getuid());
  //   // unpack the integer given in `data`
  //   // 2. deserialize argument data
  //   // ext_ints_from_arg(data, &num1, &num2);
  //   offset = ext_nums_from_arg(data, &step_num, &size);
  // #else
  //   dprintf(fd, "[%s:ext_step()]:%d BEGIN\n", compart_name(), getuid());
  //   // ext_ints_from_arg(data, &num1, &num2, fd);
  //   offset = ext_stepnum_size_from_arg(data, &step_num, &size, fd);
  // #endif // ndef LC_ALLOW_EXCHANGE_FD
  //   char *world_history = malloc(data.bufc - offset);
  // #ifndef LC_ALLOW_EXCHANGE_FD
  //   ext_history_from_arg(data, offset, world_history);
  // #else
  //   ext_history_from_arg(data, offset, world_history, fd);
  // #endif // ndef LC_ALLOW_EXCHANGE_FD
  //   // dprintf(fd, "[%s:ext_step()] got nums (%d, %d)\n", compart_name(),
  //   num1,
  //   // num2); num1 += 1; num2 = num2 * 3; dprintf(fd, "[%s:ext_step()]
  //   returning
  //   // (%d, %d)\n", compart_name(), num1, num2);
  //   dprintf(fd, "[%s:ext_step()] got ", compart_name());
  //   dprintf(fd, "data.buf { .step_num=%ld, ", step_num);
  //   dprintf(fd, ".size=%ld, ", size);
  //   dprintf(fd, "...}\n");
  //   step(world_history, size, step_num);
  //   dprintf(fd, "[%s:ext_step()]:%d step computed: ", compart_name(),
  //   getuid()); print_world(fd, world_history, size, step_num); dprintf(fd,
  //   "\n");
  // #ifndef LC_ALLOW_EXCHANGE_FD
  //   // return ext_ints_to_arg(num1, num2);
  //   struct extension_data result = ext_step_to_arg(step_num, size,
  //   world_history);
  // #else
  //   // return ext_ints_to_arg(num1, num2, fd);
  //   struct extension_data result =
  //       ext_step_to_arg(step_num, size, world_history, fd);
  // #endif // ndef LC_ALLOW_EXCHANGE_FD
  //   free(world_history);
  //
  //   dprintf(fd, "[%s:ext_step()]:%d END\n", compart_name(), getuid());
  //   return result;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * (De)Serializers
 * -------
 *
 * for inter-compart comms
 * * * * * * * * * * * * * * * * * * * * * * * */

// pack (serialize to bytes) a given step number (integer)
// & world history (size int & string of states)
// data {
//   //     8 bytes   8 bytes  4 bytes       size_history bytes
//   //     ul        ul       size_t        char*
//   buf: { step_num, size   , size_history, world_history }
// }
#ifndef LC_ALLOW_EXCHANGE_FD
struct extension_data ext_step_to_arg(unsigned long step_num,
                                      unsigned long size, size_t size_history,
                                      char *world_history)
#else
struct extension_data ext_step_to_arg(unsigned long step_num,
                                      unsigned long size, size_t size_history,
                                      char *world_history, int fd)
#endif // ndef LC_ALLOW_EXCHANGE_FD
{
  // create empty result value
  struct extension_data result;
  // declare size of buffer to be memory size of
  // int + int + step number * size * size
  size_t size_step_num = sizeof(step_num);
  size_t size_size = sizeof(size);
  size_t size_size_history = sizeof(size_history);

  result.bufc = size_step_num + size_size + size_size_history + size_history;

  char *cur = packbuf(result.buf, (char *)&step_num, size_step_num);
  cur = packbuf(cur, (char *)&size, size_size);
  cur = packbuf(cur, (char *)&size_history, size_size_history);
  packbuf(cur, world_history, size_history);

#ifdef LC_ALLOW_EXCHANGE_FD
  result.fdc = 1;
  printf("(%d) ext_step_to_arg fd=%d\n", getpid(), fd);
  result.fd[0] = fd;
#endif // LC_ALLOW_EXCHANGE_FD
  // return result object
  return result;
}

// unpack a step number (integer), size (integer), & world history (string of
// states) from a given argument & store each in given pointers
// data {
//   //     8 bytes   8 bytes  4 bytes       size_history bytes
//   //     ul        ul       size_t        char*
//   buf: { step_num, size   , size_history, world_history }
// }
#ifndef LC_ALLOW_EXCHANGE_FD
size_t ext_nums_from_arg(struct extension_data data,
                         unsigned long *step_num_ptr, unsigned long *size_ptr,
                         size_t *size_history)
#else
size_t ext_nums_from_arg(struct extension_data data,
                         unsigned long *step_num_ptr, unsigned long *size_ptr,
                         size_t *size_history, int *fd)
#endif // ndef LC_ALLOW_EXCHANGE_FD
{
  size_t size_step_num = sizeof(*step_num_ptr);
  size_t size_size = sizeof(*size_ptr);
  size_t size_size_history = sizeof(*size_history);

  char *cur = unpackbuf(data.buf, (char *)step_num_ptr, size_step_num);
  cur = unpackbuf(cur, (char *)size_ptr, size_size);
  unpackbuf(cur, (char *)size_history, size_size_history);

#ifdef LC_ALLOW_EXCHANGE_FD
  // FIXME assert result.fdc == 1;
  *fd = data.fd[0];
#endif // LC_ALLOW_EXCHANGE_FD
  return size_step_num + size_size + size_size_history;
}

// unpack a step number (integer), size (integer), & world history (string of
// states) from a given argument & store each in given pointers
// data {
//   //     8 bytes   8 bytes  4 bytes       size_history bytes
//   //     ul        ul       size_t        char*
//   buf: { step_num, size   , size_history, world_history }
// }
#ifndef LC_ALLOW_EXCHANGE_FD
void ext_history_from_arg(struct extension_data data, size_t size_history,
                          char *world_history)
#else
void ext_history_from_arg(struct extension_data data, size_t size_history,
                          char *world_history, int *fd)
#endif // ndef LC_ALLOW_EXCHANGE_FD
{
  size_t offset = data.bufc - size_history;
  char *cur = data.buf + offset;
  memcpy(world_history, cur, size_history);
  // read history states from buffer
#ifdef LC_ALLOW_EXCHANGE_FD
  // FIXME assert result.fdc == 1;
  *fd = data.fd[0];
#endif // LC_ALLOW_EXCHANGE_FD
}

char *packbuf(char *buf, char *data, size_t len) {
  memcpy(buf, data, len);

  return buf + len;
}

char *unpackbuf(char *buf, char *data, size_t len) {
  memcpy(data, buf, len);

  return buf + len;
}
