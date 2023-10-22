#ifndef DYNARR_H
#define DYNARR_H

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAP 10

/**
 * dynamic array header. Contains array length, capacity and elements size. 
 * It is located at the beginning of the dynarr struct and is followed by the 
 * actual array of data.
 */
struct dynarr_header {
    size_t length;
    size_t capacity;
    size_t elem_size;
};

/**
 * Defines an anonymous struct that holds the dynarr_header followed by
 * the array of data of type T.
 *
 * PARAMS: 
 *  - T must be a valid type (i.e. int, char *, double, ...)
 */
#define DYNARR(T) struct { struct dynarr_header header; T *data; }

/**
 * Initializes an empty dynarr defined with DYNARR(T).
 *
 * PARAMS: 
 *  - a is defined with DYNARR(T)
 */
#define dynarr_init(a) { \
    a.header.length = 0; \
    a.header.capacity = DEFAULT_CAP; \
    a.header.elem_size = sizeof(*a.data); \
    a.data = malloc(sizeof(*a.data) * DEFAULT_CAP); \
}

/**
 * Add an element to a dynarr.
 *
 * PARAMS: 
 *  - a must be a dynarr initialized with dynarr_init(a)
 *  - elem must be a variable of dynarr type T
 */
#define dynarr_add(a, elem) { \
    if (a.header.length >= a.header.capacity) { \
        a.data = realloc(a.data, a.header.elem_size * a.header.capacity * 2); \
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

/**
 * Remove i-th element from a dynarr.
 *
 * PARAMS:
 *  - a must be a dynarr initialized with dynarr_init(a)
 *  - i is the index to remove
 */
#define dynarr_remove(a, i) { \
    if (i < 0 || i >= a.header.length) return 0; \
    for (int j = i + 1; j < a.header.length; j++) { \
        a.data[j - 1] = a.data[j]; \
    } \
    a.header.length--; \
}

/**
 * Return i-th element of a dynarr.
 *
 * PARAMS:
 *  - a must be a dynarr initialized with dynarr_init(a)
 *  - i is the index to retrieve
 *  - elem must be of dynarr type T, and will contain the retrieved value
 */
#define dynarr_get(a, i, elem) { \
    if (i < 0 || i >= a.header.length) { \
        fprintf(stderr, "error: %s:%d Array out of bound\n", __FILE__, \
                                                             __LINE__); \
        return -1; \
    } \
    elem = a.data[i]; \
}

/**
 * Set i-th element of a dynarr to a new value.
 *
 * PARAMS:
 *  - a must be a dynarr initialized with dynarr_init(a)
 *  - i is the index to set  
 *  - elem is the new value
 */
#define dynarr_set(a, i, elem) { \
    if (i < 0 || i >= a.header.length) { \
        fprintf(stderr, "error: %s:%d Array out of bound\n", __FILE__, \
                                                             __LINE__); \
        return -1; \
    } \
    a.data[i] = elem; \
}

/**
 * Transform a dynarr to a fixed size array. The dynarr is not free'd.
 * It also returns the length of the array.
 *
 * PARAMS:
 *  - a must be a dynarr initialized with dynarr_init(a)
 *  - arr must be a pointer to a fixed size array of the dynarr type T
 */
#define dynarr_to_arr(a, __arr) ({ \
    arr = malloc(a.header.capacity * a.header.elem_size); \
    memcpy(arr, a.data, a.header.elem_size * a.header.length); \
    int __size = a.header.length; \
    __size; \
})

/**
 * Free a dynarr.
 *
 * PARAMS:
 *  - a must be a dynarr initialized with dynarr_init(a)
 */
#define dynarr_free(a) { \
    if (a.data) { \
        free(a.data); \
    } \
}

#endif
