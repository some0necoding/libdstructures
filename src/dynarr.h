#include <stdint.h>
#include <stdlib.h>

/**
 * All possible elem types. Any of its members corresponds to a member of the
 * union inside dynarr_entry.
 */
enum elem_type {
    PTR,
    BIT_8,
    BIT_16,
    BIT_32,
    BIT_64
};

/**
 * Represents an elem inside the dynamic array. To allow a generic-ish usage the
 * elem can be of different types, specified by the elem_type enum, each one
 * associated with a union member.
 */
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

/**
 * Represents a dynamic array instance. It contains a reference to the array
 * of elements, its length (i.e. number of elements it contains) and its capacity
 * (i.e. the current array size).
 * The entries are allocated contiguously.
 */
typedef struct {
    dynarr_entry** entries;
    size_t len;
    size_t cap;
} dynarr;

/**
 * Create a new dynarr instance.
 *
 * @param size initial size of the array. If it is not a power of 2 it is rounded
 *             to the next one.
 * @return pointer to the new dynarr instance if no error occurs; NULL and errno
 *         set if memory allocation fails.
 */
dynarr* dynarr_new(size_t size);

/**
 * Append an element at the end of the array. If the element is a pointer the
 * memory region it points to is not copied; the dynarr only keeps a reference
 * to it.
 *
 * @param arr the dynarr instance appending to
 * @param elem pointer or 8/16/32/64 bit value to insert at the end of the array
 * @return 0 no error;
 *         1 memory allocation failed
 */
uint8_t dynarr_appendptr(dynarr* arr, void* elem);
uint8_t dynarr_append8  (dynarr* arr, uint8_t elem);
uint8_t dynarr_append16 (dynarr* arr, uint16_t elem);
uint8_t dynarr_append32 (dynarr* arr, uint32_t elem);
uint8_t dynarr_append64 (dynarr* arr, uint64_t elem);

/**
 * Store an element in a particular position of the array. If the element is a
 * pointer the memory region it points to is not copied; the dynarr only keeps a
 * reference to it.
 *
 * @param arr the dynarr instance to modify
 * @param i the index where elem must be inserted; must be in [0, len)
 * @param elem pointer or 8/16/32/64 bit value to insert at the specified index
 * @return 0 no error;
 *         1 index out of range
 */
uint8_t dynarr_setptr(dynarr* arr, size_t i, void* elem);
uint8_t dynarr_set8  (dynarr* arr, size_t i, uint8_t elem);
uint8_t dynarr_set16 (dynarr* arr, size_t i, uint16_t elem);
uint8_t dynarr_set32 (dynarr* arr, size_t i, uint32_t elem);
uint8_t dynarr_set64 (dynarr* arr, size_t i, uint64_t elem);

/**
 * Get an element in a particular position of the array.
 *
 * @param arr the dynarr instance to get the element from
 * @param i the index where the element is; must be in [0, len)
 * @param elem pointer to pointer or 8/16/32/64 bit value where the value at the
 *             specified index will be stored
 * @return 0 no error;
 *         1 index out of range
 */
uint8_t dynarr_getptr(dynarr* arr, size_t i, void** elem);
uint8_t dynarr_get8  (dynarr* arr, size_t i, uint8_t* elem);
uint8_t dynarr_get16 (dynarr* arr, size_t i, uint16_t* elem);
uint8_t dynarr_get32 (dynarr* arr, size_t i, uint32_t* elem);
uint8_t dynarr_get64 (dynarr* arr, size_t i, uint64_t* elem);

/**
 * Get the current length of the array.
 *
 * @param arr the dynarr instance to get the length of
 * @return the current length of the array
 */
size_t  dynarr_size(dynarr* arr);

void    dynarr_qsort(dynarr* arr, int (*compar)(const void*, const void*));
