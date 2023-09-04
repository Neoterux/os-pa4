#include <stddef.h>
#include <semaphore.h>
#include <stdint.h>
#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__ 1

/**
 * @brief This structure contains the basic information about
 * an instance of shared memory.
 *
 */
struct shmem_attr_t {
  void *shmem_ptr;
  const sem_t *access_sem;
};

typedef struct shmem_attr_t shmem_attr_t;

/**
 * @brief Map the given output file as shared memory, it wil
 *
 * @param __out The output file that will be shared across process
 * @param required_space The required space for the file
 * @return int The file descriptor of the shared memory
 */
int configure_shared_memory(const char *restrict __out, size_t required_space, shmem_attr_t *restrict _out);

int write_on_shared_image();

/**
 * @brief Configure pipe
 *
 * @return int
 */
int configure_pipe();

int start_magic();

void mperror(const char *msg);

#endif /* __PROCESSOR_H__ */
