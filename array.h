#ifndef __PHYSICS_ARRAY_H
#define __PHYSICS_ARRAY_H
#include <stdarg.h>
#include <stdlib.h>

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
    void dynamic_##arr_name##_push(struct dynamic_##arr_name **a, size_t count, arr_type *elements) {   \
        size_t length = (*a)->length;                                           \
        if (length + count > (*a)->size) {                                      \
            size_t size = (*a)->size * 2;                                       \
            *a = realloc(*a, size);                                             \
            (*a)->size = size;                                                  \
        }                                                                       \
                                                                                \
        for (size_t i = 0; i < count; i++) {                                    \
            (*a)->elements[length + i] = elements[i];                           \
        }                                                                       \
                                                                                \
        (*a)->length += count;                                                  \
    }                                                                           

#endif // __PHYSICS_ARRAY_H
