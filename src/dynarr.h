#ifndef LIBDSTRUCTURES_DYNARR_H
#define LIBDSTRUCTURES_DYNARR_H

#include <stdint.h>
#include <stdlib.h>

struct dynarr_type {
    void *(*valDup)(void *val);
    void (*valFree)(void *val);
    /*
     * @param valCompare the comparison function that will be applied to the
     *        entries of the array and that must return:
     *          - 0 on equality
     *          - < 0 if val1 < val2
     *          - > 0 if vas1 > val2
     */
    int (*valCompare)(const void *val1, const void *val2);
};

struct dynarr_entry {
    void *val;
};

typedef struct {
    struct dynarr_type *type;
    struct dynarr_entry** entries;
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
 * Append an element at the end of the array.
 *
 * @param arr the dynarr instance appending to
 * @param val the value to insert
 * @return 0 no error;
 *         1 memory allocation failed
 *         2 resizing failed
 *         3 val duplication failed
 */
uint8_t dynarr_append(dynarr* arr, void* val);

/**
 * Store an element in a particular position of the array.
 *
 * @param arr the dynarr instance to modify
 * @param i the index where val must be inserted; must be in [0, len)
 * @param val the value to insert
 * @return 0 no error;
 *         1 index out of range
 *         3 val duplication failed
 */
uint8_t dynarr_set(dynarr* arr, size_t i, void* val);

/**
 * Get an element in a particular position of the array.
 *
 * @param arr the dynarr instance to get the element from
 * @param i the index where the element is; must be in [0, len)
 * @param val returned value
 * @return 0 no error;
 *         1 index out of range
 */
uint8_t dynarr_get(dynarr* arr, size_t i, void** val);

/**
 * Remove an element from a particular index of the array.
 *
 * @param arr the array to remove the element from
 * @param i the index of the element to remove
 * @return 0 no error;
 *         1 memory allocation failed
 *         2 resizing failed
 *         3 index out of range
 */
uint8_t dynarr_remove(dynarr* arr, size_t i);

/**
 * Get the current length of the array.
 *
 * @param arr the dynarr instance to get the length of
 * @return the current length of the array
 */
size_t  dynarr_size(dynarr* arr);

/**
 * Sort an array.
 *
 * @param arr the array to sort
 * @return 0 no error
 *         1 memory allocation failed
 *         2 no compare function
 */
uint8_t dynarr_sort(dynarr* arr);

/**
 * Free a dynarr instance.
 *
 * @param arr the dynamic array to free
 */
void dynarr_free(dynarr* arr);

#endif // !LIBDSTRUCTURES_DYNARR_H
