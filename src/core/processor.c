#include "processor.h"
#include "../utils/log.h"
#include "bits/errors.h"
#include "common_types.h"
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHMEM_SEMNAME "IMAGE_ACCESS_SEM"

/**
 * @brief The settings of the program.
 *
 */
extern progopt_t settings;

int merr_no = 0;

int shmid;

void orchestor();

int configure_pipe() { return 0; }

int configure_shared_memory(const char *restrict output, size_t size, shmem_attr_t *config) {
  /* The configuration of the shared memory (Singleton) */
  // static shmem_attr_t config = { NULL, NULL };
  if (output == NULL) {
    merr_no = INVALID_SHMEM_FILENAME;
    return -1;
  }


  int fd = open(output, O_CREAT | O_RDWR | O_TRUNC, S_IWUSR | S_IWUSR | S_IRUSR);
  if (fd == -1) {
    perror("couldn't create shared memory [123]");
    exit(EXIT_FAILURE);
  }
  int result = lseek(fd, size - 1, SEEK_SET);
  if (result == -1) {
    close(fd);
    perror("Error when 'stretch' file");
    exit(EXIT_FAILURE);
  }
  result = write(fd, "", 1);
  if (result != 1) {
    close(fd);
    perror("Error while writing last byte on output");
    exit(EXIT_FAILURE);
  }
  // Initialize the shared memory file to write into
  void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  config->access_sem = sem_open(SHMEM_SEMNAME, O_CREAT, S_IRUSR | S_IWUSR, 1);
  config->shmem_ptr = ptr;
  close(fd);
  return 1;
}

int start_magic() { return 0; }

void mperror(const char *msg) {
  char *decode_msg = NULL;
  switch (merr_no) {
  case ERROR_PIPE:
    decode_msg = "Error creating the pipe";
    break;
  case ERROR_SHMM:
    decode_msg = "Error creating the shared memory";
    break;
  }
  log_err("%s: %s\n", msg, decode_msg);
}
