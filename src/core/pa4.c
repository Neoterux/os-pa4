#include <stdio.h>
#include "pa4.h"

void version(char *exec) {
    printf(
        "%s - %s\n"
        "Written by Neoterux\n", exec, PA4_VERSION);
}

void usage(char *exec) {
    printf("Usage: %s [options] <input_file> <output_file>\n", exec);
    printf(
        "OPTIONS:\n"
        "-v|--version\tPrint the version information.\n"
        "-h|--help\tPrint this message.\n"
        "-w|--workers <n>\tSetup the worker thread count for image processing (for each processor). "
            "If set <= 0 then the value will be the same as CPU count. [default=4]\n"
        "-p|--preview\tShow the preview of the images on terminals that can support it. [default=false]\n"
        "-V|--vertical\tApply the filter on vertical. [default=false]\n"
        "-i|--inverse\tApply the filter inverting the order of filters [default=false].\n"
    );
}
