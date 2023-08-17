#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "core/pa4.h"

struct progopt settings = {
    .threads = 4,
    .horizontal = 1,
    .inverse = 0,
    .preview = 0,
};

/**
 * @brief
 * This is the entry point of the program.
 * Here we will parse the given parameters from
 * the CLI and configure the image processor.
 *
 * The given TODO list is:
 * - Parse the arguments (and setup for the optional)
 * - Check the given input image and make the transformation
 * - Check the output and validate if the output should be
 *   overwritten when there is a file already named like that.
 * - Create an IPC System to communicate through the filter
 *   subprocess.
 * - Create the filters subprocess, and they should be able to
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

    return 0;
}
