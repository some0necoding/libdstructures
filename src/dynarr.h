#include <stdint.h>
#include <stdlib.h>

typedef struct {
    void** arr;
    size_t len;
    size_t cap;
} dynarr;

dynarr* dynarr_new(size_t size);
uint8_t dynarr_add(dynarr* arr, void* elem);
uint8_t dynarr_set(dynarr* arr, size_t i, void* elem);
uint8_t dynarr_get(dynarr* arr, size_t i, void** elem);
size_t  dynarr_size(dynarr* arr);
void    dynarr_qsort(dynarr* arr, int (*compar)(const void*, const void*));
