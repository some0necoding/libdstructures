#include "dynarr.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

dynarr* dynarr_new(size_t size) {
    dynarr* arr = calloc(1, sizeof(dynarr));
    if (!arr) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__, __LINE__, errno);
        return NULL;
    }

    arr->arr = calloc(size, sizeof(void*));
    if (!arr->arr) {
        fprintf(stderr, "%s:%d: cannot allocate memory [errno: %d]\n", __FILE__, __LINE__, errno);
        return NULL;
    }

    arr->cap = size;
    arr->len = 0;
    return arr;
}

int dynarr_add(dynarr *arr, void *elem) {
    if (arr->len >= arr->cap - 1) {
        size_t new_cap = arr->cap * 2;
        arr->arr = reallocarray(arr->arr, new_cap, sizeof(void*));
        if (!arr->arr) {
            fprintf(stderr, "%s:%d: cannot reallocate memory [errno: %d]\n", __FILE__, __LINE__, errno);
            return -1;
        }
        arr->cap = new_cap;
    }

    arr->arr[arr->len++] = elem;
    return 0;
}

int dynarr_set(dynarr* arr, size_t i, void* elem) {
    if (i < 0 || i >= arr->len) {
        fprintf(stderr, "%s:%d: index %lu out of bound\n", __FILE__, __LINE__, i);
        return -1;
    }

    arr->arr[i] = elem;
    return 0;
}

void* dynarr_get(dynarr *arr, size_t i) {
    if (i < 0 || i >= arr->len) {
        fprintf(stderr, "%s:%d: index %lu out of bound\n", __FILE__, __LINE__, i);
        return NULL;
    }

    return arr->arr[i];
}

size_t dynarr_size(dynarr* arr) {
    return arr->len;
}

void dynarr_qsort(dynarr *arr, int (*compar)(const void *, const void *)) {
    qsort(arr->arr, arr->len, sizeof(void*), compar);
}
