#ifndef __COMMON_TYPES_H__
#define __COMMON_TYPES_H__ 1

typedef unsigned char bool;

struct progopt {
    int threads;
    bool inverse;
    bool horizontal;
    bool preview;
};

typedef struct progopt progopt_t;

#endif /* __COMMON_TYPES_H__ */
