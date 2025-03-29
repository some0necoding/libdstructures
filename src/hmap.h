#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>


/**
 * All possible value types. Any of its members corresponds to a member of the
 * union inside hmap_entry.
 */
enum value_type {
    PTR,
    BIT_8,
    BIT_16,
    BIT_32,
    BIT_64
};


/**
 * Represents an entry inside the hashmap. To allow a generic-ish usage:
 *  - The key is a sequence of bytes stored in the given memory region of the
 *    specified size. Based on this assumption, the comparison between two keys
 *    is done by first checking their length and, if equal, by doing a memcmp on
 *    the two.
 *  - The value can be of different types, specified by the value_type enum,
 *    each one associated with a union member.
 */
typedef struct {
    void* key;
    size_t key_size;
    enum value_type type;
    union {
        void*    value_ptr;
        uint8_t  value_8;
        uint16_t value_16;
        uint32_t value_32;
        uint64_t value_64;
    };
} hmap_entry;

/**
 * Represents a hashmap instance. It contains a reference to the hash function
 * used (murmur3) and the dynamic array that contains the entries.
 * Collisions are handled with linear probing.
 */
typedef struct {
    uint32_t (*hash)(const void*, size_t);
    hmap_entry** entries;
    size_t len;
    size_t cap;
} hmap;

/**
 * Create a new hmap with the specified initial size. This is the only way to
 * create an hmap instance. All hmap instances must be freed using hmap_free().
 *
 * @param size the initial size of the hmap. If it is not a power of 2 it gets
 *             rounded to the next one.
 * @return     a pointer to an hmap struct with the specified initial size if no
 *             error occurs. NULL is returned and errno set if memory cannot be
 *             allocated.
 */
hmap*    hmap_new(size_t size);

/**
 * Put a new entry in the hmap with the specified key and a pointer value or
 * a 8/16/32/64 bit value.
 * The memory region the pointer value points to does not get copied, the map
 * only stores the memory reference; instead the key is copied, and that's why
 * key_size is needed.
 *
 * @param map the map to update
 * @param key pointer of the memory region that contains the key
 * @param key_size size of the memory region that contains the key
 * @param value pointer or 8/16/32/64 bit value
 * @return 0 if no error occurs;
 *         1 if memory allocation fails;
 *         2 if rehash fails
 */
uint8_t  hmap_putptr(hmap* map, void* key, size_t key_size, void*   value);
uint8_t  hmap_put8  (hmap* map, void* key, size_t key_size, uint8_t  value);
uint8_t  hmap_put16 (hmap* map, void* key, size_t key_size, uint16_t value);
uint8_t  hmap_put32 (hmap* map, void* key, size_t key_size, uint32_t value);
uint8_t  hmap_put64 (hmap* map, void* key, size_t key_size, uint64_t value);

/**
 * Retrieve a pointer value or 8/16/32/64 bit value by its key.
 *
 * @param map the map to lookup in
 * @param key pointer to the memory region that contains the key to lookup for
 * @param key_size size of the memory region that contains the key to lookup for
 * @param value pointer to a variable that will contain the lookup result. If
 *              the lookup fails its value is undefined.
 * @return 0 no error;
 *         1 key not found
 */
uint8_t  hmap_getptr(hmap* map, void* key, size_t key_size, void** value);
uint8_t  hmap_get8  (hmap* map, void* key, size_t key_size, uint8_t* value);
uint8_t  hmap_get16 (hmap* map, void* key, size_t key_size, uint16_t* value);
uint8_t  hmap_get32 (hmap* map, void* key, size_t key_size, uint32_t* value);
uint8_t  hmap_get64 (hmap* map, void* key, size_t key_size, uint64_t* value);

/**
 * Remove from the map the entry with the specified key.
 *
 * @param map the map to remove from
 * @param key pointer to the memory region that contains the key to lookup for
 * @param key_size size of the memory region that contains the key to lookup for
 * @return 0 no error;
 *         1 key not found
 */
uint8_t  hmap_remove(hmap* map, void* key, size_t key_size);

/**
 * Return a copy of all entries of the map in a dense array. The returned array,
 * the hmap_entry structs it contains, the key field of each hmap_entry struct
 * are all heap allocated and should be freed after use.
 *
 * @param map           the map entries will be extracted from
 * @param entries       pointer to an array of pointers to hmap_entry structs. It
 *                      will be allocated by the function and will contain the
 *                      map entries.
 * @param entries_size  pointer to a variable where the size of the entries
 *                      array will be stored.
 * @return 0 no error;
 *         1 memory allocation fails
 */
uint8_t hmap_entries(hmap* map, hmap_entry*** entries, size_t* entries_size);

/**
 * Free an hmap instance.
 *
 * @param map the map to be freed
 */
void hmap_free(hmap* map);
