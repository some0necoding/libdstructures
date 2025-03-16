#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>


enum value_type {
    PTR,
    BIT_8,
    BIT_16,
    BIT_32,
    BIT_64
};


typedef struct {
    void* key;  // consider using a defined type (would like to have both integers and strings)
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

typedef struct {
    uint32_t (*hash)(const void*, size_t);
    hmap_entry** entries;
    size_t len;
    size_t cap;
} hmap;


hmap*    hmap_new(size_t size);

uint8_t  hmap_putptr(hmap* map, void* key, size_t key_size, void*   value);
uint8_t  hmap_put8  (hmap* map, void* key, size_t key_size, uint8_t  value);
uint8_t  hmap_put16 (hmap* map, void* key, size_t key_size, uint16_t value);
uint8_t  hmap_put32 (hmap* map, void* key, size_t key_size, uint32_t value);
uint8_t  hmap_put64 (hmap* map, void* key, size_t key_size, uint64_t value);

uint8_t  hmap_getptr(hmap* map, void* key, size_t key_size, void** value);
uint8_t  hmap_get8  (hmap* map, void* key, size_t key_size, uint8_t* value);
uint8_t  hmap_get16 (hmap* map, void* key, size_t key_size, uint16_t* value);
uint8_t  hmap_get32 (hmap* map, void* key, size_t key_size, uint32_t* value);
uint8_t  hmap_get64 (hmap* map, void* key, size_t key_size, uint64_t* value);

int  hmap_remove(hmap* map, void* key, size_t key_size);
void hmap_free(hmap* map);
