#ifndef __PHYSICS_ARRAY_H
#define __PHYSICS_ARRAY_H
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define DEFINE_DYNAMIC_ARRAY(arr_type, arr_name)                                \
    struct dynamic_##arr_name {                                                 \
        size_t          size;                                                   \
        size_t          length;                                                 \
        arr_type        elements[];                                             \
    };                                                                          \
                                                                                \
    struct dynamic_##arr_name *init_dynamic_##arr_name() {                      \
        size_t n = 10;                                                          \
        struct dynamic_##arr_name *a = malloc(sizeof(struct dynamic_##arr_name) + sizeof(arr_type[n]));     \
        a->size = n;                                                            \
        a->length = 0;                                                          \
                                                                                \
        return a;                                                               \
    }                                                                           \
                                                                                \
    void destroy_dynamic_##arr_name(struct dynamic_##arr_name **a) {            \
        memset(*a, 0, sizeof(struct dynamic_##arr_name) + sizeof(arr_type[(*a)->size]));    \
        free(*a);                                                               \
        *a = NULL;                                                              \
    }                                                                           \
                                                                                \
    void dynamic_##arr_name##_push(struct dynamic_##arr_name **a, size_t count, arr_type *elements) {   \
        size_t length = (*a)->length;                                           \
        if (length + count > (*a)->size) {                                      \
            size_t expand_factor = 2;                                           \
            while (length + count > (*a)->size * expand_factor) {               \
                expand_factor *= 2;                                             \
            }                                                                   \
                                                                                \
            size_t size = (*a)->size * expand_factor;                           \
            *a = realloc(*a, sizeof(struct dynamic_##arr_name) + sizeof(arr_type[size]));   \
            (*a)->size = size;                                                  \
        }                                                                       \
                                                                                \
        for (size_t i = 0; i < count; i++) {                                    \
            (*a)->elements[length + i] = elements[i];                           \
        }                                                                       \
                                                                                \
        (*a)->length += count;                                                  \
    }                                                                           \
                                                                                \
    arr_type dynamic_##arr_name##_pop(struct dynamic_##arr_name **a) {          \
        size_t length = (*a)->length;                                           \
        arr_type *e = &(*a)->elements[length - 1];                              \
        arr_type popped = *e;                                                   \
        memset(e, 0, sizeof(arr_type));                                         \
        /* TODO: decrease size of array if it is now lte half size */           \
        return popped;                                                          \
    }

#endif // __PHYSICS_ARRAY_H
