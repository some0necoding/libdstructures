#include <stdint.h>
#include <stdlib.h>

enum elem_type {
    PTR,
    BIT_8,
    BIT_16,
    BIT_32,
    BIT_64
};

typedef struct {
    enum elem_type type;
    union {
        void* elem_ptr;
        uint8_t elem_8;
        uint16_t elem_16;
        uint32_t elem_32;
        uint64_t elem_64;
    };
} dynarr_entry;

typedef struct {
    dynarr_entry** entries;
    size_t len;
    size_t cap;
} dynarr;

dynarr* dynarr_new(size_t size);

uint8_t dynarr_appendptr(dynarr* arr, void* elem);
uint8_t dynarr_append8  (dynarr* arr, uint8_t elem);
uint8_t dynarr_append16 (dynarr* arr, uint16_t elem);
uint8_t dynarr_append32 (dynarr* arr, uint32_t elem);
uint8_t dynarr_append64 (dynarr* arr, uint64_t elem);

uint8_t dynarr_setptr(dynarr* arr, size_t i, void* elem);
uint8_t dynarr_set8  (dynarr* arr, size_t i, uint8_t elem);
uint8_t dynarr_set16 (dynarr* arr, size_t i, uint16_t elem);
uint8_t dynarr_set32 (dynarr* arr, size_t i, uint32_t elem);
uint8_t dynarr_set64 (dynarr* arr, size_t i, uint64_t elem);

uint8_t dynarr_getptr(dynarr* arr, size_t i, void** elem);
uint8_t dynarr_get8  (dynarr* arr, size_t i, uint8_t* elem);
uint8_t dynarr_get16 (dynarr* arr, size_t i, uint16_t* elem);
uint8_t dynarr_get32 (dynarr* arr, size_t i, uint32_t* elem);
uint8_t dynarr_get64 (dynarr* arr, size_t i, uint64_t* elem);

size_t  dynarr_size(dynarr* arr);
void    dynarr_qsort(dynarr* arr, int (*compar)(const void*, const void*));
