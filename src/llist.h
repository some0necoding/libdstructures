#ifndef LLIST_H
#define LLIST_H

#include <stddef.h>
#include <stdlib.h>

struct llist_char_elem {
    char __value;
    struct llist_char_elem *next;
};

struct llist_int_elem {
    int __value;
    struct llist_int_elem *next;
};

struct llist_float_elem {
    float __value;
    struct llist_float_elem *next;
};

struct llist_double_elem {
    double __value;
    struct llist_double_elem *next;
};

struct llist_ptr_elem {
    void *__value;
    struct llist_ptr_elem *next;
};

#define llist_init(l, value) ({ \
    int err = 0; \
    l = malloc(sizeof *l); \
    if (!l) { \
        err = -1; \
    } else { \
        l->__value = value; \
        l->next = NULL; \
    } \
    err; \
})

#define llist_add(T, l, value) ({ \
    int err = 0; \
    struct T *__new_elem; \
    struct T *__l = l; \
    if (llist_init(__new_elem, value) != 0) { \
        err = -1; \
    } else { \
        while (__l->next) { \
            __l = __l->next; \
        } \
        __l->next = __new_elem;\
    } \
    err; \
})

#endif
