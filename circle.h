#ifndef __PHYSICS_CIRCLE_H
#define __PHYSICS_CIRCLE_H
#include "array.h"
#include "linalg.h"
#include "position.h"
#include "velocity.h"

struct Circle {
    float               radius;
    float               mass;
    struct Velocity     velocity;
    struct Position     position;
};

DEFINE_DYNAMIC_ARRAY(struct Circle, struct_Circle_array);

// Fill a buffer of floats to represent the transforms of all struct Circle in `*a`.
//
// This function assumes the buffer is large enough to fit all transforms.
// This function will transpose the matrix in preparation for OpenGL usage.
void dynamic_struct_Circle_array_to_transform_buffer(struct dynamic_struct_Circle_array *a, float *out_buffer) {
    size_t count = a->length;
    for (size_t i = 0; i < count; i++) {
        size_t buffer_i = i * 16;
        mat4 T;
        mat4_translation(T, (vec3){ a->elements[i].position.x, a->elements[i].position.y, 0.0f });
        mat4_transpose(T, T);
        for (size_t j = 0; j < 16; j++) {
            out_buffer[buffer_i + j] = T[j / 4][j % 4];
        }
    }
}

#endif // __PHYSICS_CIRCLE_H
