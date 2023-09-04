#include <bits/pthreadtypes.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>
#include "core/bits/errors.h"
#include "core/pa4.h"
#include "core/processor.h"
#include "utils/log.h"
#include "core/common_types.h"
#include "files/image.h"
#include <wait.h>
#include "concurrency/threadpool.h"
#include "filters/kernel.h"
#include "filters/convolution.h"

#define TP_QUEUE_SIZE 1024

struct progopt settings = {
    .threads = 6,
    .horizontal = 1,
    .inverse = 0,
    .preview = 0,
};

struct work_cfg_t {
  uint32_t start_row_idx;
  uint32_t end_row_idx;
  uint32_t start_col_idx;
  uint32_t end_col_idx;
};
struct runner_cfg_t {
  int pixel_row, pixel_col;
  kernel_t kernel;
  int multipler;
  sem_t *done_lock;
};
struct conc_cfg_t {
  threadpool_t *pool;
  pthread_mutex_t lock;
};

typedef struct work_cfg_t work_cfg_t;

typedef struct conc_cfg_t conc_cfg_t;

Image *origin_image = NULL;
const char *SHMEM_KEY = "SOURCE_IMAGE";
size_t pixel_offset;
shmem_attr_t shmem;
int started = 0, done = 0;

pthread_mutex_t worker_lock;


void write_into_shared_memory(Pixel *content, int row, int col ) {
  write_on_image(shmem.shmem_ptr, origin_image, *content, row, col);

}

void get_around_matrix(int_fast16_t (*target)[3], channel_t channel, const int row, const int col) {
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      int nrow = (origin_image->height + row + i) % origin_image->height;
      int ncol = (origin_image->width + col + j) % origin_image->width;
      Pixel px = get_pixel_at(origin_image, nrow, ncol);
      if (channel == BLUE) target[i+1][j+1] = px.blue;
      else if (channel == GREEN) target[i+1][j+1] = px.green;
      else if (channel == RED) target[i+1][j+1] = px.red;
      else log_err("Unknown channel was given to extract\n");
    }
  }

}

int lock = 10;
void process_kernel(void *content) {
  struct runner_cfg_t *args = content;
  Pixel pxl = get_pixel_at(origin_image, args->pixel_row, args->pixel_col);
  Pixel out = { 0, 0, 0, pxl.alpha };
  int_fast16_t dest[3][3];
  for(channel_t channel = BLUE; channel <= RED; channel++) {
    get_around_matrix(dest, channel, args->pixel_row, args->pixel_col);
    uint8_t result = apply_filter(args->kernel, args->multipler, dest);
    if (channel == BLUE) out.blue = result;
    else if (channel == GREEN) out.green = result;
    else if (channel == RED) out.red = result;
    else {
      log_err("Unknown channel detected\n");
    }
  }
  if (--lock > 0)
      log_debug(
        "%d=======| Pixel Original { g: %d b: %d r: %d a: %d } > Pixel modificado { g: %d b: %d r: %d a: %d } | ======\n", lock,
        pxl.blue, pxl.green, pxl.red, pxl.alpha,
        out.blue, out.green, out.alpha
      );
  write_into_shared_memory(&out, args->pixel_row, args->pixel_col);
  // log_warn("Running xd\n");
  pthread_mutex_lock(&worker_lock);
  done++;
  // sem_post(args->done_lock);
  pthread_mutex_unlock(&worker_lock);
}


void blur_process(work_cfg_t cfg, int thread_count) {
  sem_wait(shmem.start); // Lock for blur process
  threadpool_t *pool = NULL;
  sem_t pool_sem;
  sem_init(&pool_sem, 0, thread_count);
  log_info("Launching blur\n");
  log_debug("blur is supposed to run from (r: %d, c: %d) to (r: %d, c: %d)\n",
    cfg.start_row_idx, cfg.start_col_idx,
    cfg.end_row_idx, cfg.end_col_idx
  );
  pthread_mutex_init(&worker_lock, NULL);
  pool = threadpool_create(thread_count, TP_QUEUE_SIZE, 0);
  int tasks = 0;
  for(int row = cfg.start_row_idx; row <= cfg.end_row_idx; row++) {
    for (int col = cfg.start_col_idx; col <= cfg.end_col_idx; col++) {
      struct runner_cfg_t *args = malloc(sizeof(struct runner_cfg_t));
      // memcpy(args->kernel, RIDGE_KERNEL, sizeof(kernel_t));
      // args->multipler = 1;
      memcpy(args->kernel, GAUSSIAN_BLUR_3X3_KERNEL, sizeof(kernel_t));
      args->multipler = GAUSSIAN_BLUR_3X3_KERNEL_FACTOR;
      // memcpy(args->kernel, BOX_BLUR_KERNEL, sizeof(kernel_t));
      // args->multipler = BOX_BLUR_KERNEL_MULTIPLIER;
      args->pixel_col = col;
      args->pixel_row = row;
      args->done_lock = &pool_sem;
      // sem_wait(&pool_sem);
      if (threadpool_add(pool, process_kernel, args, 0) == 0){
        // log_debug("adding thread\n");
        pthread_mutex_lock(&worker_lock);
        tasks++;
        // log_debug("New worker thread created\n");
        pthread_mutex_unlock(&worker_lock);
      } else {
        log_warn("The queue is full, waiting...\n");
        int max_intents = 100;
        while(threadpool_add(pool, process_kernel, args, 0) != 0 && max_intents) {
          sleep(1);
          max_intents--;
        }
        if (max_intents == 0) {
          log_err("queue renew was exhausted");
          exit(EXIT_FAILURE);
        }
      }
    }
  }
  log_info("started %d threads\n", tasks);
  log_info("done %d threads\n", done );
  // while (threadpool_add(pool, process_kernel, void *arg, int flags))
  threadpool_destroy(pool, 0);
  log_info("blur process has finnished\n");
}


void border_process(work_cfg_t cfg, int thread_count) {
  sem_wait(shmem.start); // Lock for blur process
  threadpool_t *pool = NULL;
  sem_t pool_sem;
  sem_init(&pool_sem, 0, thread_count);
  log_info("Launching border\n");
  log_debug("border is supposed to run from (r: %d, c: %d) to (r: %d, c: %d)\n",
    cfg.start_row_idx, cfg.start_col_idx,
    cfg.end_row_idx, cfg.end_col_idx
  );
  pthread_mutex_init(&worker_lock, NULL);
  pool = threadpool_create(thread_count, TP_QUEUE_SIZE, 0);
  int tasks = 0;
  for(int row = cfg.start_row_idx; row <= cfg.end_row_idx; row++) {
    for (int col = cfg.start_col_idx; col <= cfg.end_col_idx; col++) {
      struct runner_cfg_t *args = malloc(sizeof(struct runner_cfg_t));
      memcpy(args->kernel, RIDGE_KERNEL, sizeof(kernel_t));
      args->multipler = 1;
      // memcpy(args->kernel, GAUSSIAN_BLUR_3X3_KERNEL, sizeof(kernel_t));
      // args->multipler = GAUSSIAN_BLUR_3X3_KERNEL_FACTOR;
      // memcpy(args->kernel, BOX_BLUR_KERNEL, sizeof(kernel_t));
      // args->multipler = BOX_BLUR_KERNEL_MULTIPLIER;
      args->pixel_col = col;
      args->pixel_row = row;
      args->done_lock = &pool_sem;
      // sem_wait(&pool_sem);
      if (threadpool_add(pool, process_kernel, args, 0) == 0){
        // log_debug("adding thread\n");
        pthread_mutex_lock(&worker_lock);
        tasks++;
        // log_debug("New worker thread created\n");
        pthread_mutex_unlock(&worker_lock);
      } else {
        log_warn("The queue is full, waiting...\n");
        int max_intents = 100;
        while(threadpool_add(pool, process_kernel, args, 0) != 0 && max_intents) {
          sleep(1);
          max_intents--;
        }
        if (max_intents == 0) {
          log_err("queue renew was exhausted");
          exit(EXIT_FAILURE);
        }
      }
    }
  }
  log_info("started %d threads\n", tasks);
  log_info("done %d threads\n", done );
  // while (threadpool_add(pool, process_kernel, void *arg, int flags))
  threadpool_destroy(pool, 0);
  log_info("blur process has finnished\n");
}

void cleanup_semaphores(void);

/**
 * @brief
 * This is the entry point of the program.
 * Here we will parse the given parameters from
 * the CLI and configure the image processor.
 *
 * The given TODO list is:
 * - [x] Parse the arguments (and setup for the optional)
 * - [~] Check the given input image and make the transformation
 * - [x] Check the output and validate if the output should be
 *   overwritten when there is a file already named like that.
 * - [~] Create an IPC System to communicate through the filter
 *   subprocess.
 * - [ ] Create the filters subprocess, and they should be able to
 *   prevent a deadlock
 *
 */
int main(int argc, char **argv) {
    static struct option log_opts[] = {
        { "workers", optional_argument, NULL, 'w' },
        { "help", optional_argument, NULL, 'h' },
        { "version", optional_argument, NULL, 'v' },
        { "preview", optional_argument, NULL, 'p' },
        { "vertical", optional_argument, NULL, 'V' },
        { "inverse", optional_argument, NULL, 'i' },
        {  0, 0, 0, 0 },
    };

    int c;
    int option_idx = 0;
    while ((c = getopt_long(argc, argv, "hvw:Vpi", log_opts, &option_idx)) != -1) {
        switch(c) {
            case 'w':
                errno = 0;
                char *endptr;
                settings.threads = strtol(optarg, &endptr, 10);
                if (errno != 0) {
                    switch(errno) {
                        case EINVAL:
                         fprintf(stderr, "Not supported value was given for worker count\n");
                         break;
                         case ERANGE:
                         fprintf(stderr, "Worker quantity out of range\n");
                         break;
                    }
                    exit(EXIT_FAILURE);
                }
                if (endptr == optarg){
                    fprintf(stderr, "Invalid worker quantity: '%s'\n", optarg);
                    exit(EXIT_FAILURE);
                }
                if (settings.threads <= 0) {
                    settings.threads = sysconf(_SC_NPROCESSORS_ONLN);
                }
                break;
            case 'p':
                settings.preview = 1;
                break;
            case 'i':
                settings.inverse = 1;
                break;
            case 'V':
                log_info("Configuring to apply filters at horizontal way (WARN!!!)");
                settings.horizontal = 0;
                break;
            case 'v':
                version(argv[0]);
                exit(EXIT_SUCCESS);
            case 'h':
                usage(argv[0]);
                exit(EXIT_SUCCESS);
            case '?':
                usage(argv[0]);
                exit(EXIT_FAILURE);
            default:
            break;
        }
    }
    const int nargc = argc - optind;
    if (nargc < 2) {
        usage(argv[0]);
        fprintf(stderr, "Missing image(s) paths\n");
        exit(EXIT_FAILURE);
    }
    if (nargc > 2) {
        fprintf(stderr, "Invalid argument: '%s'\n", argv[optind + 2]);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    // setup the working process
    log_debug("Setting up the working process\n");
    log_debug("Threads per process: %d\n", settings.threads);

    /*
    * Now we are going to create a pipe to communicate between the child process
    * and the parent process.
    */
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("Couldn't create pipe");
        exit(EXIT_FAILURE);
    }

    // We will read the image from the given path
    const char* image_path = argv[optind];
    const char* output_path = argv[optind + 1];

    origin_image = read_image(image_path, NULL);
    if (image_errno != 0) {
        iperror("Error when reading the image");
        exit(EXIT_FAILURE);
    }
    /*
    * Now, configuring the shared memory to read the image data from the same source
    * between the 2 consumers (2 fork processes) and the parent (producer).
    */
    log_debug("Trying to configure shared memory\n");
    size_t required_space = get_required_space(origin_image);
    log_info("reserving shared memory %lld bytes\n", required_space);
    configure_shared_memory(output_path, required_space, &shmem);
    if (shmem.shmem_ptr == NULL) {
      log_err("Cannot configure the shared memory\n");
      exit(EXIT_FAILURE);
    }
    // Setup to wait until all is configured and start syncronized
    sem_wait(shmem.start);
    sem_wait(shmem.start);

    // Now we are going to clone the image to the shared memory
    // copy_content_to_shared_memory(shm_id, shared_image, origin_image);
    // Now clone the content
    // const u_int8_t zero[required_space];
    void *ptr = write_image_header(shmem.shmem_ptr, origin_image);
    if (ptr == NULL) {
      log_err("Something went wrong while writting the header\n");
      exit(EXIT_FAILURE);
    }
    // memcpy(shmem.shmem_ptr, zero, required_space);

    log_info("Starting blur filter\n");
    pid_t blur_pid, border_pid;
    uint32_t mid_width = origin_image->width / 2 + (origin_image->width % 2);
    uint32_t mid_height = origin_image->height / 2 + (origin_image->width % 2);

    work_cfg_t blur_cfg = { 0, 0, 0, 0};
    work_cfg_t border_cfg = { 0, origin_image->height - 1, 0, 0};
    if (settings.horizontal) {
      blur_cfg.end_row_idx = mid_height - 1;
      border_cfg.end_col_idx = (blur_cfg.end_col_idx = origin_image->width - 1);
      border_cfg.start_row_idx = mid_height;
    } else {
      blur_cfg.end_col_idx = mid_width - 1;
      border_cfg.start_col_idx = mid_width;
      border_cfg.end_col_idx = origin_image->width - 1;
      border_cfg.end_row_idx = (blur_cfg.end_row_idx = origin_image->height - 1);
    }
    log_debug("configuration for blur: { x0: %u, y0: %u, xf: %u, yf: %u}\n", blur_cfg.start_col_idx, blur_cfg.start_row_idx, blur_cfg.end_col_idx, blur_cfg.end_row_idx);
    log_debug("configuration for border: { x0: %u, y0: %u, xf: %u, yf: %u}\n", border_cfg.start_col_idx, border_cfg.start_row_idx, border_cfg.end_col_idx, border_cfg.end_row_idx);

    switch ((blur_pid = fork())) {
      case -1:
        perror("couldn't start blur filter");
        exit(EXIT_FAILURE);
      case 0:
        blur_process(blur_cfg, 4);
        return 0;
      default:
        log_debug("blur process started with pid: %ld\n", blur_pid);
    }

    log_info("Starting border filter\n");
    switch ((border_pid = fork())) {
      case -1:
        perror("couldn't start border filter");
        exit(EXIT_FAILURE);
      case 0:
        border_process(border_cfg, 4);
        return 0;
      default:
        log_debug("border process started with pid: %ld\n", border_pid);
    }

    sem_post(shmem.start); // Now both process should start
    sem_post(shmem.start); // Unlock the 2 processors



    /**
     * Now we are going to handle the pipe messages, to
     * atomically write on image.
     */
     wait(NULL); // For the 1st Processor
     wait(NULL); // For the 2nd processor
    cleanup_semaphores();
    return 0;
}


void cleanup_semaphores(void) {
  log_debug("closing all semaphores\n");
  sem_close(shmem.write_sem);
  sem_close(shmem.blur);
  sem_close(shmem.border);
  sem_close(shmem.start);
  log_debug("unlinking semaphores\n");
  unlink_sems();
}
