#ifndef DYNARR_H
#define DYNARR_H

#include <errno.h>
#include <stdlib.h>

#define DEFAULT_CAP 10

struct dynarr_header {
    size_t length;
    size_t capacity;
    size_t elem_size;
};

#define DYNARR(T) struct { struct dynarr_header header; T *data; }

#define dynarr_init(a) { \
    a.header.length = 0; \
    a.header.capacity = DEFAULT_CAP; \
    a.header.elem_size = sizeof(*a.data); \
    a.data = malloc(sizeof(*a.data) * DEFAULT_CAP); \
}

#define dynarr_add(a, elem) { \
    if (a.header.length >= a.header.capacity) { \
        a.data = realloc(a.data, sizeof(*a.data) * a.header.capacity * 2); \
        if (!a.data) { \
            fprintf(stderr, "error: %s:%d (errno: %d) \
                            Cannot allocate memory\n", __FILE__, \
                                                       __LINE__, \
                                                       errno); \
            return -1; \
        } \
        a.header.capacity *= 2; \
    } \
    a.data[a.header.length] = elem; \
    a.header.length++; \
}

#define dynarr_get(a, i, elem) { \
    if (i < 0 || i >= a.header.length) { \
        fprintf(stderr, "error: %s:%d Array out of bound\n", __FILE__, \
                                                             __LINE__, \
                                                             errno); \
        return -1; \
    } \
    elem = a.data[i]; \
}

#define dynarr_set(a, i, elem) { \
    if (i < 0 || i >= a.header.length) { \
        fprintf(stderr, "error: %s:%d Array out of bound\n", __FILE__, \
                                                             __LINE__, \
                                                             errno); \
        return -1; \
    } \
    a.data[i] = elem; \
}

#define dynarr_remove(a, i) { \
    if (i < 0 || i >= a.header.length) return 0; \
    for (int j = i + 1; j < a.header.length; j++) { \
        a.data[j - 1] = a.data[j]; \
    } \
    a.header.length--; \
}

#define dynarr_free(a) { \
    free(a.data) \
}

#endif
