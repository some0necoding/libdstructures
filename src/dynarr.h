#include <stdlib.h>

typedef struct {
    void** arr;
    size_t len;
    size_t cap;
} dynarr;

dynarr* dynarr_new(size_t size);
int     dynarr_add(dynarr* arr, void* elem);
int     dynarr_set(dynarr* arr, size_t i, void* elem);
void*   dynarr_get(dynarr* arr, size_t i);
size_t  dynarr_size(dynarr* arr);
void    dynarr_qsort(dynarr* arr, int (*compar)(const void*, const void*));
